#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include <iostream>
#include <fstream>
#include <list>

#include "scapegoat.hpp"
#include "fastlog.hpp"

using namespace std;


//NOTE: This is the order of insertions used to produce the screenshot.
//list<int> add_order = {5, 37, 13, 58, 89, 34, 90, 96, 29, 84, 92, 40, 42, 22, 51, 3, 15, 2, 31, 47, 59, 20, 66, 14, 72, 68, 9, 88, 12, 86, 62, 75, 82, 61, 91, 81, 67, 71, 26, 35, 23, 17, 95, 16, 30, 6, 50, 21, 46, 24, 78, 93, 79, 19, 18, 11, 25, 76, 77, 28, 4, 48, 44, 56, 69, 45, 49, 63, 94, 1, 55, 10, 43, 32, 33, 36, 85, 98, 70, 74, 65, 73, 60, 64, 39, 38, 83, 54, 52, 100, 41, 8, 97, 99, 57, 27, 80, 7, 87, 53};


void loadRandom(lightweight_map<int, int>& mymap, size_t TEST_SIZE)
{
	//Create a list of X random numbers, 1...X
	list<int> randNum;
	while (randNum.size()<TEST_SIZE) {
		randNum.push_back(randNum.size()+1);
	}

	//Add them one-by-one (randomly) to a lightweight map
	std::cout <<"Add: {";
	srand (time(nullptr));
	while (!randNum.empty()) {
		list<int>::iterator it = randNum.begin();
		std::advance(it, (rand() % randNum.size()));

		std::cout <<*it <<(randNum.size()>1?", ":"");

		mymap.insert(*it, *it);
		randNum.erase(it);
	}

	std::cout <<"}" <<endl;
}

void deleteRandom(lightweight_map<int, int>& mymap, double amt)
{
	size_t TEST_SIZE = mymap.size();
	std::cout <<"Remove: {";
	while (mymap.size()>TEST_SIZE-(int)(TEST_SIZE*amt)) {
		int toDelete = rand() % TEST_SIZE;
		mymap.remove(toDelete);

		std::cout <<toDelete <<(mymap.size()>0?", ":"");
	}
	std::cout <<"}" <<endl;
}



int main()
{
	//Load random.
/*	lightweight_map<int, int> mymap;
	loadRandom(mymap, 100);

	//Now randomly delete a certain percentage of that tree.
	deleteRandom(mymap, 0.2);

	//Add a few more
	loadRandom(mymap, 10);*/

	//Specific tests
	/*list<int> add_order = {5, 37, 13, 58, 89, 34, 90, 96, 29, 84, 92, 40, 42, 22, 51, 3, 15, 2, 31, 47, 59, 20, 66, 14, 72, 68, 9, 88, 12, 86, 62, 75, 82, 61, 91, 81, 67, 71, 26, 35, 23, 17, 95, 16, 30, 6, 50, 21, 46, 24, 78, 93, 79, 19, 18, 11, 25, 76, 77, 28, 4, 48, 44, 56, 69, 45, 49, 63, 94, 1, 55, 10, 43, 32, 33, 36, 85, 98, 70, 74, 65, 73, 60, 64, 39, 38, 83, 54, 52, 100, 41, 8, 97, 99, 57, 27, 80, 7, 87, 53};
	for (auto id : add_order) {
		std::cout <<"Inserting node: " <<id <<"\n";
		mymap.insert(id, id);
	}*/
	/*list<int> rem_order = {64, 12, 61, 38, 41, 4, 18, 96, 26, 91, 71, 19, 53, 79, 30, 49, 18, 51, 26, 50, 87, 51, 91, 61, 97, };
	for (auto id : rem_order) {
		mymap.remove(id);
	}*/

	//Print a GraphVis input file for this tree.
	/*if (!mymap.printDot("tree.dot")) {
		std::cout <<"Error: Couldn't print file.\n";
	}*/


	//Fast log test
	float alpha = 1.0F / 0.55F;
	fast_log<int> logB2(2);
	fast_log<float> logAlpha(alpha);
	for (size_t i=0; i<1000; i++) {
		int toCheckI = rand() % 1000+1;
		float toCheckF = toCheckI / 0.33F;

		//Get actual logs
		float actLog2 = log2(toCheckI);
		float actLogAlpha = log(toCheckF)/log(alpha);

		//Compare floor/ceiling: log2
		if (logB2.floor(toCheckI) != ((int)floor(actLog2))) {
			std::cout <<"Floor<int> mismtach.\n";
		}
		if (logB2.ceil(toCheckI) != ((int)ceil(actLog2))) {
			std::cout <<"Ceiling<int> mismtach.\n";
		}

		//Compare floor/ceiling: logAlpha
		if (logAlpha.floor(toCheckF) != ((int)floor(actLogAlpha))) {
			std::cout <<"Floor<float> mismtach.\n";
		}
		if (logAlpha.ceil(toCheckF) != ((int)ceil(actLogAlpha))) {
			std::cout <<"Ceiling<float> mismtach.\n";
		}
	}


	cout <<"Done\n";
	return 0;
}
