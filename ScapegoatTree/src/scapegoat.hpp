#pragma once

//Define this to add a function called "printJson()", which uses STL filestreams
#ifdef SCAPEGOAT_TREE_ALLOW_OUTPUT
#include <fstream>
#include <string>
#endif

//Needed for log2
#include <cmath>



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


public:
	lightweight_map() : root(nullptr), realSize(0) {}

	//The tunable alpha parameter determines how "unbalanced" the binary tree can become
	// before a scapegoat is found and the entire tree balanced. It ensures that
	// size(root->left) < alpha*size(root), and the same for root->right.
	//Thus, an alpha of 0.5 represents a perfectly balanced tree, while an alpha
	// of 1.0 considers a linked-list-esque (worst case) tree balanced.
	//Obviously, setting this closer to 0.5 will slow down insertions.
	void setAlpha(double value) {
		if (value<0.5) {
			alpha = 500;
		} else if (value>1.0) {
			alpha = 1000;
		} else {
			alpha = (size_t)(value*1000);
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
	void setAutoBalance(bool val, bool forceRebalance) {
		autoBalance = val;
		if (autoBalance && forceRebalance) {
			rebalance(root);
		}
	}

	void insert(Key key, Data value) {
		recurse(key, nullptr, root, Action::Insert)->data = value;
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
		recurse(key, nullptr, root, Action::Delete);
	}

	size_t size() {
		return realSize;
	}

	//TEMP:
	void test_rebalance(Key key) {
		node* res = recurse(key, nullptr, root, Action::Find);
		if (res) {
			//NOTE: Finding the parent will not be necessary in the final algorithm, since
			//      it will be part of the Scapegoat lookup.
			node* parent = nullptr;
			if (root!=res) {
				parent = root;
				while (parent->left!=res && parent->right!=res) {
					if (key<parent->key) {
						parent = parent->left;
					} else {
						parent = parent->right;
					}
				}
			}

			rebalance(parent, res);
		}
	}

private:
	//Conceptually: Turn our tree into the worst possible binary search tree. Then turn that
	//  into the best-possible binary search tree.
	void rebalance(node* parent, node* from) {
		//Check if we're replacing the root node.
		//bool resetRoot = (root==from);

		//Probably easiest to just hijack our existing node* structure.
		node*& sectionStart = !parent?root:parent->left==from?parent->left:parent->right;
		node* curr = from;
		node* prev = parent;
		size_t len = 0;
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
		int numCompressions = (int)log2(len)-1;
		while (numCompressions-- > 0) {
			std::cout <<"start is: " <<sectionStart->key <<"\n";

			//Iterate down the entire RHS of the tree
			prev = parent;
			for (curr=sectionStart; curr;) {
				//Deal with pairs of nodes.
				node* oldCurr = curr;
				curr = curr->right;

				//Update the nodes themselves
				if (curr) {
					std::cout <<"   swapping: " <<oldCurr->key <<" and " <<curr->key <<"\n";

					//Update the parent pointer
					node*& parentPtr = !prev?root:prev->left==oldCurr?prev->left:prev->right;
					parentPtr = curr;
					prev = curr;

					//Swap
					oldCurr->right = curr->left;
					curr->left = oldCurr;

					//Increment
					curr = curr->right;
				}
			}
		}
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


	node* recurse(Key key, node* parent, node* curr, Action action) {
		//Base case: No more nodes
		if (!curr) {
			//If we're searching or deleting, then we do nothing. For insertion, this is a valid
			//  location for a new node.
			if (action==Action::Find || action==Action::Delete) {
				return nullptr;
			} else if (action==Action::Insert) {
				realSize++;
				curr = new node(key);

				//Add to parent
				if (!parent) {
					root = curr;
				} else if (curr->key<parent->key) {
					parent->left = curr;
				} else {
					parent->right = curr;
				}

				return curr;
			}
		}

		//Base case: Node found
		if (curr->key==key) {
			//If we're searching or inserting, return this node. If we're deleting, check.
			if (action==Action::Find || action==Action::Insert)  {
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
		if (key<curr->key) {
			return recurse(key, curr, curr->left, action);
		} else {
			return recurse(key, curr, curr->right, action);
		}
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


private:
	//BST parameters
	node* root;
	size_t realSize;

	//Parameters
	size_t alpha;   //*1000
	bool rigidDelete;
	bool autoBalance;

	//Scapegoat tree parameters


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


