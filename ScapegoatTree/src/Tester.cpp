#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <fstream>
#include <list>

#include "scapegoat.hpp"

using namespace std;


int main()
{
	//Create a list of X random numbers, 1...X
	const size_t TEST_SIZE = 100;
	list<int> randNum;
	while (randNum.size()<TEST_SIZE) {
		randNum.push_back(randNum.size()+1);
	}

	//Create a lightweight map and add them one-by-one (randomly)
	lightweight_map<int, int> mymap;
	srand (time(nullptr));
	while (!randNum.empty()) {
		list<int>::iterator it = randNum.begin();
		std::advance(it, (rand() % randNum.size()));

		std::cout <<"Adding: " <<*it <<"\n";

		mymap.insert(*it, *it);
		randNum.erase(it);
	}

	//Now randomly delete roughly 1/5 of the map.
	//int maxChecks = ((TEST_SIZE/5)*3)/2;
	while (mymap.size()>TEST_SIZE-TEST_SIZE/5) {
		/*if (maxChecks--<=0) { //Avoid endless looping in buggy cases.
			break;
		}*/

		int toDelete = rand() % TEST_SIZE;

		std::cout <<"Deleting: " <<toDelete <<" from tree of size: " <<mymap.size()  <<"\n";

		mymap.remove(toDelete);
	}

	//Print it, check it.
	if (!mymap.printJson("tree.json.txt")) {
		std::cout <<"Error: Couldn't print file.\n";
	}


	cout <<"Done\n";
	return 0;
}
