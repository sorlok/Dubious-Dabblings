#pragma once

//Needed for log2, log10
//cmath links by default, so I don't think this is an onerous dependency.
//#include <cmath>

#include <nall/vector.hpp>

//Needed for "traverse"; will probably replace with nall/function later.
#include <functional>

//Faster logarithms. Avoids floating-point math altogether if the
//  base is an integer; otherwise, simplifies the log calculation down to
//  "floor" and "ceiling".
#include "fastlog.hpp"



//
//TODO LIST:
//           2. Use the "better" scapegoat selection discussed by Rivest.
//           3. Check Rivest's "fast" tree sort to see if we introduced a rounding error. Re-enable it if it'll work.
//



#ifdef SCAPEGOAT_TREE_ALLOW_OUTPUT
//Forward declaration
template <class Key, class Data>
class lightweight_map;

//Friend templates
template <class Key, class Data>
bool PrintDot(const char* fName, const lightweight_map<Key, Data>& tree);
#endif



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
	struct node {
		Key    key;     Data  data;
		node*  left;    node* right;
		node(Key key) : key(key), left(nullptr), right(nullptr) {}
	};

	struct slice_res {
		node* parent;
		node* child;
	};

	//Log lookups
	fast_log<int> log2;
	fast_log<float> logA;

	//BST parameters
	node* root;

	//Parameters
	size_t alpha;   //*1000
	bool rigidDelete;
	bool autoBalance;
	size_t minRebalanceSize;
	bool useFastRebalancing;

	//Scapegoat tree parameters
	size_t realSize;
	size_t maxSize;


	//Friends
#ifdef SCAPEGOAT_TREE_ALLOW_OUTPUT
	friend bool PrintDot <> (const char* fName, const lightweight_map<Key, Data>& tree);
#endif


public:
	lightweight_map(double alpha=0.55) : log2(2), logA(1.0/alpha), root(nullptr), rigidDelete(false), autoBalance(true), minRebalanceSize(3), useFastRebalancing(false), realSize(0), maxSize(0) {
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
	//NOTE: This must be done in the constructor now for "fastlog" to work.
	//TODO: We can simply clear the "fastlog" array; it's not a disaster. Re-enable this later.
	void setAlpha(double value) {
		//Reset log lookup
		logA.setBase(1.0/value);

		//Save
		if (value<0.5) {
			alpha = 500;
		} else if (value>1.0) {
			alpha = 1000;
		} else {
			alpha = static_cast<size_t>(value*1000);
		}
	}


	//Temporary: This will become the default as soon as it's tested. At that point I may keep
	//           the "safe" rebalancing, or I may remove it (the paper's pretty solid, it would
	//           only be my implementation that's in question.)
	void setUseFastRebalancing(bool value) {
		useFastRebalancing = value;
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
			rebalance(nullptr, root, realSize);
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
		nall::linear_vector<node*> parentStack;
		parentStack.append(nullptr);
		insert_r(key, parentStack, root, 0)->data = value;
	}

	bool find(Key key, Data& result) {
		node* res = find_r(key, nullptr, root);
		if (res) {
			result = res->data;
			return true;
		}
		return false;
	}

	Key& rootKey() {
		return root->key;
	}

	void remove(Key key) {
		remove_r(key, nullptr, root);
	}

	void for_each(std::function<void (const Key& key, Data& data)> action) {
		traverse(root, action);
	}

	size_t size() {
		return realSize;
	}

private:
	void traverse(node* curr, std::function<void (const Key& key, Data& data)> action) {
		//Perform for the current node
		action(curr->key, curr->data);

		//Recurse
		if (curr->left) {
			traverse(curr->left, action);
		}
		if (curr->right) {
			traverse(curr->right, action);
		}
	}

	//Conceptually: Turn our tree into the worst possible binary search tree. Then turn that
	//  into the best-possible binary search tree.
	void rebalance(node* parent, node* from, size_t nodeSize) {
		//If we're doing a full rebalance, reset the size to avoid rebalancing on the
		//  next delete.
		if (!parent) {
			maxSize = realSize;
		}

		//Pick "fast" or "simple" depending on the flag set.
		if (useFastRebalancing) {
			tree_rebalance_fast(parent, from, nodeSize);
		} else {
			tree_rebalance_simple(parent, from, nodeSize);
		}
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

	//AncestorID starts at zero and increases by one for every time we jump up to an ancestor.
	void findAndBalanceScapegoat(nall::linear_vector<node*>& parentStack, node* curr, size_t nodeSize, size_t ancestorID) {
		bool foundScapegoat = false;
		while (!foundScapegoat) {
			//Step 1: Pop the stack, compute the parent size, and replace curr with parent; update ancestorID
			node* parent = parentStack[parentStack.size()-1];
			parentStack.resize(parentStack.size()-1);
			size_t siblingSize = calc_size(parent->left==curr?parent->right:parent->left);
			nodeSize = nodeSize + siblingSize + 1;
			curr = parent;
			ancestorID++;

			//Step 2: Check if this node is the root; if not, apply the formula.
			//        (Note that the root node satisfies the formula, but we want to be extra-safe here).
			foundScapegoat = (curr==root);
			if (!foundScapegoat) {
				foundScapegoat = (ancestorID > logA.log(nodeSize));
			}

			//Step 3: Rebalance if this is the scapegoat
			if (foundScapegoat) {
				rebalance(parentStack[parentStack.size()-1], curr, nodeSize);
			}
		}
	}


	//Find a node recursively
	node* find_r(Key key, node* parent, node* curr) {
		//Base case: Nothing found
		if (!curr) {
			return nullptr;
		}

		//Base case: Node found
		if (curr->key==key) {
			return curr;
		}

		//Recursive case
		if (key<curr->key) {
			return find_r(key, curr, curr->left);
		} else {
			return find_r(key, curr, curr->right);
		}
	}



	//Remove a node, recursive version
	//NOTE: Since removing a node can rebalance the tree, ensure that NO processing occurs
	//      after any recursive call.
	node* remove_r(Key key, node* parent, node* curr) {
		//Base case: Not found
		if (!curr) {
			return nullptr;
		}

		//Base case: Node found
		if (curr->key==key) {
			//Obtain a reference to this parent's pointer (left or right) that points to this object.
			node*& parentPtr = !parent?root:parent->left==curr?parent->left:parent->right;

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

			//Update size
			realSize--;

			//Check if rebalancing is necessary
			if (autoBalance && realSize>=minRebalanceSize) {
				bool outOfBalance = false;
				if (!rigidDelete) {
					//Check if our size is less than half the max size (alpha modifies this slightly)
					outOfBalance = realSize < (alpha*maxSize)/1000;
				} else {
					//Check if the size is one less than an exact power of two
					outOfBalance = !(realSize&(realSize+1));
				}

				//If so, rebalance at the root and reset maxSize
				if (outOfBalance) {
					rebalance(nullptr, root, realSize);
				}
			}

			//Return null
			return nullptr;
		}

		//Recursive case
		if (key<curr->key) {
			return remove_r(key, curr, curr->left);
		} else {
			return remove_r(key, curr, curr->right);
		}
	}




	//Inserting a node, recursive version
	//NOTE: Since inserting a node can rebalance the tree, ensure that NO processing occurs
	//      after any recursive call.
	node* insert_r(Key key, nall::linear_vector<node*>& parentStack, node* curr, size_t nodeHeight) {
		//Base case: No more nodes
		if (!curr) {
			//If nothing found, this is the valid position for a new node.
			realSize++;
			maxSize = realSize>maxSize?realSize:maxSize;
			curr = new node(key);

			//Add to the parent
			node* parent = parentStack[parentStack.size()-1];
			node*& parentPtr = !parent?root:key<parent->key?parent->left:parent->right;
			parentPtr = curr;

			//Balance; check threshhold
			if (autoBalance && realSize>=minRebalanceSize) {
				//Dirty math hack:
				size_t thresh = logA.log(realSize);

				//From Rivest's paper: We know the tree is not height-balanced if:
				if (++nodeHeight>thresh) {
					//NOTE: This will rebalance the tree; do NOTHING except return after this.
					findAndBalanceScapegoat(parentStack, curr, 1, 0);
				}
			}

			return curr;
		}

		//Base case: Node found
		if (curr->key==key) {
			//"Insert" here means inserting a node that already exists, so we
			// don't need to check for a scapegoat.
			return curr;
		}

		//Recursive case
		parentStack.append(curr);
		if (key<curr->key) {
			return insert_r(key, parentStack, curr->left, nodeHeight+1);
		} else {
			return insert_r(key, parentStack, curr->right, nodeHeight+1);
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


	//////////////////////////////////////////////////////
	//"Simple" tree rebalancer and related functions
	//////////////////////////////////////////////////////

	void tree_rebalance_simple(node* parent, node* from, size_t nodeSize) {
		node temp(0);
		node* flatRoot = flatten(from, &temp);
		buildTree(nodeSize, flatRoot);

		//The only thing left to do is update the parent.
		node*& sectionStart = !parent?root:parent->left==from?parent->left:parent->right;
		sectionStart = temp.left;
	}

	node* flatten(node* start, node* store) {
		if (!start) {
			return store;
		}
		start->right = flatten(start->right, store);
		return flatten(start->left, start);
	}

	node* buildTree(int nodeSize, node* curr) {
		//Base case.
		if (nodeSize==0) {
			curr->left = nullptr;
			return curr;
		}

		//Recursive case
		node* r = buildTree(u_ceil(nodeSize-1, 2), curr);
		node* s = buildTree(u_floor(nodeSize-1, 2), r->right);
		r->right = s->left;
		s->left = r;
		return s;
	}



	//////////////////////////////////////////////////////
	//"Fast" tree rebalancer and related functions
	//////////////////////////////////////////////////////

	//What type of node are we inserting?
	enum class InsertType { Left, Right, NonLeaf };

	//Lightweight class for stacks of a fixed size.
		template <class T>
		struct simple_stack {
			simple_stack(size_t size) : sp(0), maxsz(size) {
				entries = new T[size];
			}
			~simple_stack() {
				if (sp!=0) {
					std::cout <<"ERROR: Stack deleted with extra entries\n"; //TEMP
				}
				delete [] entries;
			}

			void push(const T& val) {
				if (sp==maxsz) {
					std::cout <<"ERROR: Stack overflow\n";  //TEMP
					throw 1;
				}
				entries[sp++] = val;
			}

			T pop() {
				if (sp==0) {
					std::cout <<"ERROR: Stack underflow\n"; //TEMP
					throw 1;
				}
				return entries[--sp];
			}

			T& top() {
				if (sp==0) {
					std::cout <<"ERROR: Stack is empty\n"; //TEMP
					throw 1;
				}
				return entries[sp-1];
			}

			const T& second() {
				if (sp<=1) {
					std::cout <<"ERROR: Stack has <=1 element\n"; //TEMP
					throw 1;
				}
				return entries[sp-2];
			}

		private:
			T* entries;
			size_t sp;
			size_t maxsz;
		};

	struct builditem {
		node* currNode;
		size_t height;
		bool lacksRightSon;
		bool lacksFather;
	};

	struct balanceargs {
		int slotsInLastLevel;
		int nodesForLastLevel;
		double ratio;
		simple_stack<node*> runningStack;
		simple_stack<builditem> buildingStack;
	};

	void tree_rebalance_fast(node* parent, node* from, size_t nodeSize) {
		//Simple parameters
		InsertType iType = InsertType::Left;
		balanceargs args = {
			static_cast<int>(power(2, static_cast<int>(log2.log(static_cast<int>(nodeSize))))),
			0,
			0,
			simple_stack<node*>(logA.log(static_cast<float>(nodeSize))+2),
			simple_stack<builditem>(log2.log(static_cast<int>(nodeSize))+1)
		};
		args.nodesForLastLevel = nodeSize - args.slotsInLastLevel + 1;
		args.ratio = ((double)args.nodesForLastLevel)/args.slotsInLastLevel;

		//Stack sizes are bound as follows
		simple_stack<node*> runningStack();
		simple_stack<node*> buildingStack();

		//Begin!
		args.runningStack.push(from);
		while (nodeSize-- > 0) {
			iType = addNewNode(getNextNode(args), iType, args);
		}

		//The only thing left to do is update the parent.
		node*& sectionStart = !parent?root:parent->left==from?parent->left:parent->right;
		sectionStart = args.buildingStack.pop().currNode;
	}

	node* getNextNode(balanceargs& args) {
		//Pick the next node to calculate (in-order). Determine its parent.
		node* nextNode = args.runningStack.top();
		node* fatherNode = nullptr;
		while (nextNode->left) {
			fatherNode = nextNode;
			nextNode = nextNode->left;
		}

		//Slice this node from its parent node, unless it's at the top of the stack (then pop it)
		if (nextNode == args.runningStack.top()) {
			args.runningStack.pop();
		} else {
			fatherNode->left = nullptr;
		}

		//If there are right children, they will need to be processed later.
		if (nextNode->right) {
			args.runningStack.push(nextNode->right);
		}

		return nextNode;
	}


	InsertType addNewNode(node* nextNode, InsertType iType, balanceargs& args) {
		if (iType != InsertType::NonLeaf) {
			//Inserting a leaf node
			args.slotsInLastLevel--;
			if (((double)args.nodesForLastLevel)/args.slotsInLastLevel < args.ratio) {
				return skipALeaf(nextNode, iType, args);
			} else {
				args.nodesForLastLevel--;
				return addALeaf(nextNode, iType, args);
			}
		} else {
			return addNonLeaf(nextNode, args);
		}
	}

	InsertType skipALeaf(node* nextNode, InsertType iType, balanceargs& args) {
		if (iType==InsertType::Left) {
			nextNode->left = nullptr;
			if (args.buildingStack.top().height==2) {
				args.buildingStack.top().currNode->right = nextNode;
				if (!args.buildingStack.top().lacksFather) {
					args.buildingStack.pop();
				} else {
					args.buildingStack.top().lacksRightSon = false;
				}
				args.buildingStack.push({nextNode, 1, true, false});
			} else {
				args.buildingStack.push({nextNode, 1, true, true});
			}
			return InsertType::Right;
		} else {
			args.buildingStack.top().currNode->right = nullptr;
			if (!args.buildingStack.top().lacksFather) {
				args.buildingStack.pop();
			} else {
				args.buildingStack.top().lacksRightSon = false;
			}
			return addNonLeaf(nextNode, args);
		}
	}

	InsertType addALeaf(node* nextNode, InsertType iType, balanceargs& args) {
		nextNode->right = nullptr;
		nextNode->left = nullptr;
		if (iType==InsertType::Left) {
			args.buildingStack.push({nextNode, 0, false, true});
		} else {
			if (args.buildingStack.top().lacksFather) {
				args.buildingStack.top().lacksRightSon = false;
			} else {
				args.buildingStack.pop();
			}
		}
		return InsertType::NonLeaf;
	}

	InsertType addNonLeaf(node* nextNode, balanceargs& args) {
		nextNode->left = args.buildingStack.top().currNode;
		int nextNodeHeight = args.buildingStack.top().height + 1;
		args.buildingStack.pop();
		if (args.buildingStack.second().height == nextNodeHeight+1) {
			if (!args.buildingStack.top().lacksFather) {
				args.buildingStack.pop();
			} else {
				args.buildingStack.top().lacksRightSon = false;
			}
			args.buildingStack.push({nextNode, nextNodeHeight, true, false});
		} else {
			args.buildingStack.push({nextNode, nextNodeHeight, true, true});
		}
		if (args.buildingStack.top().height > 1) {
			return InsertType::Left;
		} else {
			return InsertType::Right;
		}
	}

};




//Define this to add output functions, which use STL filestreams
#ifdef SCAPEGOAT_TREE_ALLOW_OUTPUT
#include "scapegoat-display.hpp"
#endif

