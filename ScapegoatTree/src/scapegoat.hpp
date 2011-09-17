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

	node* recurse(Key key, node* parent, node*& curr, Action action) {
		//Base case: No more nodes
		if (!curr) {
			//If we're searching or deleting, then we do nothing. For insertion, this is a valid
			//  location for a new node.
			if (action==Action::Find || action==Action::Delete) {
				return nullptr;
			} else if (action==Action::Insert) {
				realSize++;
				curr = new node(key);
				return curr;
			}
		}

		//Base case: Node found
		if (curr->key==key) {
			//If we're searching or inserting, return this node. If we're deleting, check.
			if (action==Action::Find || action==Action::Insert)  {
				return curr;
			} else if (action==Action::Delete) {
				//Retrieve a free node that should replace this node (and then be deleted)
				// (We favor the left side of the tree)
				node* toDelete = find_and_slice_replacement(parent, curr, true);

				//Now, replace this node with the one we found (the children do not change though)
				if (curr!=toDelete) { //Avoid unneeded copying.
					curr->data = toDelete->data;
					curr->key = toDelete->key;
				}

				//At this point, we can safely delete this node, and return null.
				realSize--;
				delete curr;
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

	node* find_and_slice_replacement(node* parent, node* curr, bool first) {
		//All three cases (1 child, 2 children, no children) can be handled at once
		//  if managed carefully
		bool slice = false;
		if (first) {
			//How many children?
			if (!curr->left || !curr->right) {
				parent = curr;
				curr = curr->left?curr->left:curr->right?curr->right:curr;
				slice = true;
			} else {
				curr = find_and_slice_replacement(curr, curr->left, false);
			}
		} else {
			//Find right-most branch
			if (curr->right) {
				curr = find_and_slice_replacement(curr, curr->right, false);
			} else {
				slice = true;
			}
		}

		//Slice it
		if (slice) {
			if (parent->left==curr) {
				parent->left = nullptr;
			} else if (parent->right==curr) {
				parent->right = nullptr;
			} //else: we're at the root node. (See code above.)
		}

		return curr;
	}


#ifdef SCAPEGOAT_TREE_ALLOW_OUTPUT
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
#endif


private:
	node* root;
	size_t realSize;
};


