#pragma once


template <class Key, class Data>
class lightweight_map {
private:
	enum class Action  { Find, Insert, Delete };

	struct node {
		Key    key;     Data  data;
		node*  left;    node* right;
		node() : left(nullptr), right(nullptr) {}
	};


public:
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

	node* recurse(Key key, node* parent, node* curr, Action action) {
		//Base case: No more nodes
		if (!curr) {
			//If we're searching or deleting, then we do nothing. For insertion, this is a valid
			//  location for a new node.
			if (action==Action::Find || action==Action::Delete) {
				return nullptr;
			} else if (action==Action::Insert) {
				curr = new node();
				return curr;
			}
		}

		//Base case: Node found
		if (curr->key==key) {
			//If we're searching or inserting, return this node. If we're deleting, check.
			if (action==Action::Find || action==Action::Insert)  {
				return curr;
			} else if (action==Action::Delete) {
				//Favor the left node
				node* newChild = curr->left?curr->left:curr->right;
				if (!parent) {
					root = newChild;
				} else {
					if (parent->left==curr) {
						parent->left = newChild;
					} else {
						parent->right = newChild;
					}
				}
				delete curr; //NOTE: Invalid if both children are non-null
			}
		}

		//Recursive case
		if (key<curr->key) {
			return recurse(key, curr, curr->left, action);
		} else {
			return recurse(key, curr, curr->right, action);
		}
	}


private:
	node* root;
};


