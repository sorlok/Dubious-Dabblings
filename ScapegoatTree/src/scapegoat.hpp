#pragma once

//Define this to add a function called "printJson()", which uses STL filestreams
#ifdef SCAPEGOAT_TREE_ALLOW_OUTPUT
#include <fstream>
#include <string>
#endif

//Needed for log2, log10
//cmath links by default, so I don't think this is an onerous dependency.
#include <cmath>

//
//TODO: The original paper by Rivest has a good deal of analysis on performance
//      of different rebalancing/insertion/etc. algorithms. I'd like to modify
//      this code to use the best ones from that paper (in particular, a number of
//      non-recursive algorithms were identified as ideal).
//Link: http://publications.csail.mit.edu/lcs/pubs/pdf/MIT-LCS-TR-700.pdf
//


//
// A light-weight map based on Scapegoat trees, with constant-space per-node overhead.
// A quick note on tuning: You should never have to consider tuning the parameters
//   "rigidDelete" and "minRebalanceSize", except in truly extreme cases. The parameter
//   "autoRebalance" may be switched off when adding large numbers of components (and then
//   switched on again with "force" set to true after), but, again, this is unlikely to
//   matter much. The only parameter you may want to tinker with is "alpha", which by default
//   is set to a value which favors around a 20 to 1 ratio of searches to insertions. If your
//   ratio is lower, you may want to bump the alpha value up to 0.6 or 0.65.
//
// TL/DR: You can use lightweight_map with the default parameters and it will perform fine.
//
template <class Key, class Data>
class lightweight_map {
private:
	enum class Action  { Find, Insert, Delete };

	struct node {
		Key    key;     Data  data;
		node*  left;    node* right;
		node(Key key) : key(key), left(nullptr), right(nullptr) {}
	};

	struct slice_res {
		node* parent;
		node* child;
	};


	//BST parameters
	node* root;

	//Parameters
	size_t alpha;   //*1000
	bool rigidDelete;
	bool autoBalance;
	size_t minRebalanceSize;

	//Scapegoat tree parameters
	size_t realSize;
	size_t maxSize;


public:
	lightweight_map(double alpha=0.55) : root(nullptr), rigidDelete(false), autoBalance(true), minRebalanceSize(3), realSize(0), maxSize(0) {
		setAlpha(alpha);
	}

	//The tunable alpha parameter determines how "unbalanced" the binary tree can become
	// before a scapegoat is found and the entire tree balanced. It ensures that
	// size(root->left) < alpha*size(root), and the same for root->right.
	//Thus, an alpha of 0.5 represents a perfectly balanced tree, while an alpha
	// of 1.0 considers a linked-list-esque (worst case) tree balanced.
	//Obviously, setting this closer to 0.5 will slow down insertions.
	//Typical alpha values between 0.55 and 0.65 exhibit good performance. We choose 0.55 as
	// the default alpha value on the assumption that the user will typically perform more searches
	// than modifications.
	void setAlpha(double value) {
		if (value<0.5) {
			alpha = 500;
		} else if (value>1.0) {
			alpha = 1000;
		} else {
			alpha = static_cast<size_t>(value*1000);
		}
	}

	//The "rigid delete" flag allows fine-tuning deletes. When a delete is performed, the
	// tree is not rebalanced until the number of deleted nodes since the last balancing
	// equals half the total number of nodes in the tree. By setting this flag, the tree is
	// rebalanced when the difference between the total number of nodes and the number of deleted
	// nodes is one less than a power of two, which ensures that the tree remains perfectly
	// balanced.
	//In general, having a slightly unbalanced tree for deletion is not a problem, and with this
	// flag off deletion is amortized log(n). If you want better lookup performance, we would
	// recommend fiddling with the alpha parameter instead of the rigit flag.
	void setRigidDelete(bool val) {
		rigidDelete = val;
	}

	//Because of the way Scapegoat trees operate, it's possible to avoid
	// rebalancing them without affecting the overall algorithm much.
	//Thus, auto-balancing may be turned off. When switched on again, the
	// "forceRebalance" flag causes a rebalancing of the tree at the root.
	//There is still a small amount of constant-time bookkeeping that takes place
	// with this flag off. It was not deemed worthwhile to remove.
	void setAutoBalance(bool val, bool forceRebalance) {
		autoBalance = val;
		if (autoBalance && forceRebalance) {
			rebalance(root);
		}
	}

	//Set the minimum size of the tree before rebalancing takes place. If the size of the tree after
	// insertion is >= this value (which itself must be >0), then the auto-balance algorithm will
	// be run (if autoBalance is true, of course).
	//By default, this is set to 3, which is the minimum size of a tree where re-balancing will have
	// any effect. We would advise caution on setting this any higher, since storing pointers in a
	// scapegoat tree can easily lead to a worst-case insertion order (as pointers can easily be allocated
	// in increasing order). We forsee no cases where increasing this value will improve performance,
	// especially since alpha is better for fine-tuning.
	void setMinRebalanceSize(size_t val) {
		if (val>0) {
			minRebalanceSize = val;
		}
	}

	void insert(Key key, Data value) {
		bool unbalanced = false;
		size_t nodeSize = 0;
		node* scapegoat = nullptr;
		recurse(key, nullptr, root, 0, Action::Insert, unbalanced, nodeSize, scapegoat)->data = value;
	}

	bool find(Key key, Data& result) {
		node* res = recurse(key, nullptr, root, Action::Find);
		if (res) {
			result = res->data;
			return true;
		}
		return false;
	}

	void remove(Key key) {
		bool unbalanced = false; //Doesn't matter
		size_t nodeSize = 0; //Doesn't matter
		node* scapegoat = nullptr; //Doesn't matter
		recurse(key, nullptr, root, 0, Action::Delete, unbalanced, nodeSize, scapegoat);
	}

	size_t size() {
		return realSize;
	}

private:
	//Conceptually: Turn our tree into the worst possible binary search tree. Then turn that
	//  into the best-possible binary search tree.
	void rebalance(node* parent, node* from, size_t nodeSize) {
		//Probably easiest to just hijack our existing node* structure.
		node*& sectionStart = !parent?root:parent->left==from?parent->left:parent->right;
		node* curr = from;
		node* prev = parent;
		int len = 0;
		while (curr) {
			if (curr->left) {
				//Rotate the left subtree in
				curr = rotate_cw(prev, curr);
			} else {
				//Advance down the right sub-tree
				prev = curr;
				curr = curr->right;
				len++;
			}
		}

		//sectionStart->right->right...->right (until right is null) is now a linked-list.
		//Perform len-1 compressions. (This is based on several similar tree-balancing algorithms)
		//First step differs slightly if n+1 is not an exact power of 2.
		int leafCount = len + 1 - static_cast<int>(pow(2, static_cast<int>((log2(len+1)))));
		sectionStart = compress(sectionStart, leafCount);

		//Now, the normal number of compressions
		for (len=len-leafCount; len>1; len>>=1) {
			sectionStart = compress(sectionStart, len>>1);
		}
	}


	node* compress(node* start, size_t amount) {
		//Shouldn't happen, but just to be extra-safe:
		if (!start->right) {
			return start;
		}

		//Iterate down "amount" of the RHS of the tree
		node* newRoot = start->right;
		while (amount-- > 0) {
			//Get the child node; advance the parent pointer; re-position the child node.
			node* curr = start->right;
			node* next = curr->right;
			start->right = curr->left;
			curr->left = start;
			if (amount>0) {
				//Anticipate the next folding
				curr->right = next->right;
			}
			start = next;
		}

		//Return the new root so that this sub-tree can be re-assigned.
		return newRoot;
	}


	//Clockwise rotation, then return the new "root" node (left)
	node* rotate_cw(node* parent, node* at) {
		//Can't rotate with no left child
		node* pivot = at->left;
		if (!pivot) {
			return at;
		}

		//Simple
		node*& parentPtr = !parent?root:parent->left==at?parent->left:parent->right;
		at->left = pivot->right;
		pivot->right = at;
		parentPtr = pivot;
		return pivot;
	}

	size_t calc_size(node* curr) {
		if (!curr) {
			return 0;
		}

		//A little wordy, but I'd like to avoid an extra function call per leaf node.
		size_t res = 1; //Count yourself
		if (curr->left) {
			res += calc_size(curr->left);
		}
		if (curr->right) {
			res += calc_size(curr->right);
		}
		return res;
	}

	void checkScapegoat(node* parent, node* curr, size_t nodeHeight, bool& unbalanced, size_t& nodeSize, node*& scapegoat) {
		//Some computation
		size_t siblingSize = calc_size(parent->left==curr?parent->right:parent->left);
		size_t parentSize = nodeSize + siblingSize + 1;
		size_t threshhold = (alpha*parentSize)/1000;

		if (parent==root) {
			//The root node is always rebalanced
			scapegoat = parent;
		} else {
			//Nodes effectively check to see if their parents are scapegoats.
			if (nodeSize>threshhold || siblingSize>threshhold) {
				//Found a scapegoat
				scapegoat = parent;
			}
		}

		//The parent will need to know its own size for balancing/continuing the search.
		nodeSize = parentSize;
	}


	node* recurse(Key key, node* parent, node* curr, size_t nodeHeight, Action action, bool& unbalanced, size_t& nodeSize, node*& scapegoat) {
		//Base case: No more nodes
		if (!curr) {
			//If we're searching or deleting, then we do nothing. For insertion, this is a valid
			//  location for a new node.
			if (action==Action::Find || action==Action::Delete) {
				return nullptr;
			} else if (action==Action::Insert) {
				realSize++;
				maxSize = realSize>maxSize?realSize:maxSize;
				curr = new node(key);

				//Add to parent
				if (!parent) {
					root = curr;
				} else if (curr->key<parent->key) {
					parent->left = curr;
				} else {
					parent->right = curr;
				}

				//Balance
				if (autoBalance) {
					//Dirty math hack:
					double realAlpha = alpha / 1000.0;
					size_t thresh = static_cast<size_t>(log10(realSize)/log10(1/realAlpha));

					//From Rivest's paper: We know the tree is not height-balanced if:
					if (++nodeHeight>thresh) {
						//Check the threshhold
						if (realSize>=minRebalanceSize) {
							std::cout <<"Node triggered scapegoat search: " <<curr->key <<" with height: " <<nodeHeight <<" and threshhold " <<thresh <<" (rounded down from " <<(log10(realSize)/log10(1/realAlpha)) <<")" <<"\n";
							unbalanced = true;
							nodeSize = 0;
							checkScapegoat(parent, curr, nodeHeight, unbalanced, nodeSize, scapegoat);
						}
					}
				}

				return curr;
			}
		}

		//Base case: Node found
		if (curr->key==key) {
			//If we're searching or inserting, return this node. If we're deleting, check.
			if (action==Action::Find || action==Action::Insert)  {
				//"Insert" here means inserting a node that already exists, so we
				// don't need to check for a scapegoat.
				return curr;
			} else if (action==Action::Delete) {
				//Obtain a reference to this parent's pointer (left or right) that points to this object.
				node*& parentPtr = parent->left==curr?parent->left:parent->right;

				//Three posibilities (I tried generalizing them, but it took up more code).
				if (!curr->left && !curr->right) {
					//Simple case: We are deleting a node with no children; just delete it and set
					// its parent pointer to null.
					parentPtr = nullptr;
					delete curr;
				} else if (!curr->left || !curr->right) {
					//Simple case 2: Only one child. Delete this node, and have the parent point to
					// this child.
					parentPtr = curr->left?curr->left:curr->right;
					delete curr;
				} else {
					//Slightly more complex case: find the previous in-order node and copy
					//   its contens here... then delete THAT node.
					node* toDelete = find_and_slice_child(curr, curr->left);
					curr->data = toDelete->data;
					curr->key = toDelete->key;
					delete toDelete;
				}

				//Either way return null
				realSize--;
				return nullptr;
			}
		}

		//Recursive case
		node* res = nullptr;
		if (key<curr->key) {
			res = recurse(key, curr, curr->left, nodeHeight+1, action, unbalanced, nodeSize, scapegoat);
		} else {
			res = recurse(key, curr, curr->right, nodeHeight+1, action, unbalanced, nodeSize, scapegoat);
		}

		//If this tree is still unbalanced, are we the scapegoat?
		if (unbalanced) {
			if (scapegoat==curr) {
				std::cout <<"   Scapegoat found: " <<curr->key <<" with parent: " <<parent <<"\n";
				rebalance(parent, curr, nodeSize);
				unbalanced = false;
			} else {
				checkScapegoat(parent, curr, nodeHeight, unbalanced, nodeSize, scapegoat);
			}
		}

		return res;
	}

	node* find_and_slice_child(node* parent, node* curr) {
		if (curr->right) {
			//Recursive case
			return find_and_slice_child(curr, curr->right);
		} else {
			//Base case; sever from parent
			node*& parentPtr = parent->left==curr?parent->left:parent->right;

			//We are guaranteed to have no right pointer, so set the parent to point to the LEFT
			//   child (if it's null that's fine too) and return the current node.
			parentPtr = curr->left;
			return curr;
		}
	}


#ifdef SCAPEGOAT_TREE_ALLOW_OUTPUT
public:
	bool printJson(const std::string& fName) {
		std::ofstream file(fName);
		if (!file.is_open()) {
			return false;
		}
		printJsonNode(file, root, 0);
		file <<std::endl;
		file.close();
		return true;
	}

	bool printDot(const std::string& fName) {
		std::ofstream file(fName);
		if (!file.is_open()) {
			return false;
		}
		file <<"digraph Tree {" <<std::endl;
		if (root) {
			file <<"root" <<" -> " <<root->key <<";" <<std::endl;
			printDotNode(file, root, 1);
		}
		file <<"}" <<std::endl;
		file.close();
		return true;
	}

private:
	void printJsonChild(std::ofstream& file, const std::string& label, node* child, size_t tabLevel) {
		std::string tabs = std::string(tabLevel*2+1, ' ');
		file <<"\n" <<tabs <<"\"" <<label <<"\":";
		if (!child) {
			file <<"{}";
		} else {
			file <<std::endl;
			printJsonNode(file, child, tabLevel+1);
		}
	}

	void printJsonNode(std::ofstream& file, node* curr, size_t tabLevel) {
		std::string tabs = std::string(tabLevel*2, ' ');
		file <<tabs <<"{"
		 	<<"\"key\":" <<"\"" <<curr->key <<"\", "
			<<"\"value\":" <<"\"" <<curr->data <<"\",";
		printJsonChild(file, "left", curr->left, tabLevel);
		printJsonChild(file, "right", curr->right, tabLevel);
		file <<std::endl <<tabs <<"}";
	}

	void printDotNode(std::ofstream& file, node* curr, size_t tabLevel) {
		std::string tabs = std::string(tabLevel*2, ' ');
		if (curr->left) {
			file <<tabs <<curr->key <<" -> " <<curr->left->key <<";" <<std::endl;
			printDotNode(file, curr->left, tabLevel+1);
		}
		if (curr->right) {
			file <<tabs <<curr->key <<" -> " <<curr->right->key <<";" <<std::endl;
			printDotNode(file, curr->right, tabLevel+1);
		}
	}

#endif

};


