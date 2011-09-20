#pragma once

//Helper class for computing floor/ceilings of logarithms in record time.


#include <nall/vector.hpp>


#include <iostream>  //Temporary


//Helper functions, since this will take the place of cmath
//These are not as powerful, as they only handle positive numerator/denominator pairs.
static int u_floor(unsigned int num, unsigned int denom) {
	return num/denom;
}
static int u_ceil(unsigned int num, unsigned int denom) {
	return 1 + (num-1)/denom;
}


template <class T>
class fast_log {
public:
	//Our array requires at least one element: log(0)==1
	// You can instruct it to initialize more elements if you want.
	fast_log(T base, size_t numInitialElements=1) : Base(base) {
		T next = 1;
		lookup.append(1);
		while (lookup.size()<numInitialElements) {
			next = next*Base;
			lookup.append(next);
		}
	}


	//Floor/Ceiling lookup functions basically just delegate
	unsigned int floor(T value) {
		return search_log(value, Search::Floor);
	}
	unsigned int ceil(T value) {
		return search_log(value, Search::Ceiling);
	}
	unsigned int log(T value) {
		return floor(value);
	}


private:
	nall::linear_vector<T> lookup;
	const T Base;

	enum class Search { Floor, Ceiling };


	unsigned int search_log(T value, Search type) {
		//Two different ways to search. The binary_search could
		// call expand_search anyway, but we'll save time and jump there
		// now if the conditions match.
		unsigned int lastIndex = lookup.size()-1;
		if (value>lookup[lastIndex]) {
			return expand_search(value, type, lastIndex);
		} else if (value>0) {
			return binary_search(value, type, 0, lastIndex);
		} else {
			return ~0; //Error
		}
	}


	//Expand the array until the result has been found.
	unsigned int expand_search(T value, Search type, unsigned int lastIndex) {
		while (value>lookup[lastIndex]) {
			lookup.append(lookup[lastIndex++]*Base);
		}
		return found(value, type, lastIndex-1, lastIndex);
	}

	//Perform a simple binary search for the value.
	unsigned int binary_search(T value, Search type, unsigned int first, unsigned int last) {
		if (first+1 == last) {
			//Base case
			return found(value, type, first, last);
		} else {
			//Recursive case
			unsigned int candidate = (last-first)/2 + first;
			if (value < lookup[candidate]) {
				return binary_search(value, type, first, candidate);
			} else if (value > lookup[candidate]) {
				return binary_search(value, type, candidate, last);
			} else {
				//Needed if the lookup array's size is 1, or to speed up searches in general
				return candidate;
			}
		}
	}

	//Return the floor or ceiling, depending
	unsigned int found(T value, Search type, unsigned int floorIndex, unsigned int ceilIndex) {
		T floorVal = lookup[floorIndex];
		T ceilVal = lookup[ceilIndex];
		if (value==floorVal) {
			return floorIndex;
		} else if (value==ceilVal) {
			return ceilIndex;
		} else {
			return type==Search::Floor?floorIndex:ceilIndex;
		}
	}



};




