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
				//Retrieve a free node that should replace this node (and then be deleted)
				// (We favor the left side of the tree)
				slice_res toDelete = find_sliceable(parent, curr, true);

				//std::cout <<"  To delete: " <<toDelete.child->key <<"\n";

				if (toDelete.parent->left==toDelete.child) {
					toDelete.parent->left = nullptr;
				} else if (toDelete.parent->right==toDelete.child) {
					toDelete.parent->right = nullptr;
				} else if (toDelete.child==root) {
					root = nullptr;
				}

				//Now, replace this node with the one we found (the children do not change though)
				if (curr!=toDelete.child) { //Avoid unneeded copying.
					curr->data = toDelete.child->data;
					curr->key = toDelete.child->key;
				}

				//At this point, we can safely delete this node.
				realSize--;
				delete toDelete.child;
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

	slice_res find_sliceable(node* parent, node* curr, bool first) {
		//All three cases (1 child, 2 children, no children) can be handled at once
		//  if managed carefully
		if (first) {
			//std::cout <<"  Children: " <<curr->left <<" , " <<curr->right <<"\n";

			//How many children?
			if (curr->left && curr->right) {
				return find_sliceable(curr, curr->left, false);
			} else {
				node* res = curr->left?curr->left:curr->right?curr->right:curr;
				return {curr, res};
			}
		} else {
			//Find right-most branch
			if (curr->right) {
				return find_sliceable(curr, curr->right, false);
			} else {
				return {parent, curr};
			}
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

#endif

};


