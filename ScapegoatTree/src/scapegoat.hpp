#pragma once

//Define this to add a function called "printJson()", which uses STL filestreams
#ifdef SCAPEGOAT_TREE_ALLOW_OUTPUT
#include <fstream>
#include <string>
#endif



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
	node* root;
	size_t realSize;


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


