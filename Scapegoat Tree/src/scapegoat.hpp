#pragma once



template <class Data>
class lightweight_map {
public:
	lightweight_map() : root(nullptr) {}

private:
	class node {
		Data  data;
		node* left;
		node* right;
	};

	node* root;
};

