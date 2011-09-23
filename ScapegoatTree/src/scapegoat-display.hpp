#pragma once

//Do not include this file directly.

#include <fstream>
#include <string>
#include <vector>

//#include "scapegoat.hpp"


//NOTE: All internal functions broken.
namespace {

template <class Key, class Data>
void PrintJsonChild(std::ofstream& file, const std::string& label, const typename lightweight_map<Key, Data>::node* const child, size_t tabLevel) {
	std::string tabs = std::string(tabLevel*2+1, ' ');
	file <<"\n" <<tabs <<"\"" <<label <<"\":";
	if (!child) {
		file <<"{}";
	} else {
		file <<std::endl;
		printJsonNode(file, child, tabLevel+1);
	}
}

template <class Key, class Data>
void PrintJsonNode(std::ofstream& file, const typename lightweight_map<Key, Data>::node* const curr, size_t tabLevel) {
	std::string tabs = std::string(tabLevel*2, ' ');
	file <<tabs <<"{"
		<<"\"key\":" <<"\"" <<curr->key <<"\", "
		<<"\"value\":" <<"\"" <<curr->data <<"\",";
	printJsonChild(file, "left", curr->left, tabLevel);
	printJsonChild(file, "right", curr->right, tabLevel);
	file <<std::endl <<tabs <<"}";
}

template <class Key, class Data>
void PrintDotNode(std::ofstream& file, const typename lightweight_map<Key, Data>::node* const curr, size_t tabLevel) {
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

} //End anon namespace


//Simple output display functions
//NOTE: Broken
template <class Key, class Data>
bool PrintJsonBROKEN(const std::string& fName, const lightweight_map<Key, Data>& tree) {
	std::ofstream file(fName);
	if (!file.is_open()) {
		return false;
	}
	PrintJsonNode(file, tree.root, 0);
	file <<std::endl;
	file.close();
	return true;
}


//NOTE: This works
template <class Key, class Data>
bool PrintDot(const char* fName, const lightweight_map<Key, Data>& tree) {
	std::ofstream file(fName);
	if (!file.is_open()) {
		return false;
	}
	file <<"digraph Tree {" <<std::endl;
	if (tree.root) {
		file <<"root" <<" -> " <<tree.root->key <<";" <<std::endl;

		//Iterate over all nodes
		std::vector<typename lightweight_map<Key, Data>::node*> nodes(1, tree.root);
		typename lightweight_map<Key, Data>::node* curr = nullptr;
		while (!nodes.empty()) {
			//Print this node
			std::string tabs = std::string(nodes.size()*2, ' ');
			curr = nodes.back();
			nodes.pop_back();

			//Print order: left to right.
			if (curr->left) {
				file <<tabs <<curr->key <<" -> " <<curr->left->key <<";" <<std::endl;
			}
			if (curr->right) {
				file <<tabs <<curr->key <<" -> " <<curr->right->key <<";" <<std::endl;
			}

			//Stack order: right to left.
			if (curr->right) {
				nodes.push_back(curr->right);
			}
			if (curr->left) {
				nodes.push_back(curr->left);
			}
		}
	}
	file <<"}" <<std::endl;
	file.close();
	return true;
}


