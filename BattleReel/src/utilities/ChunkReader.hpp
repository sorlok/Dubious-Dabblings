#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>

class ChunkReader {
public:
	bool loadFile(const std::string& path);

	size_t getNumFiles() { return files.size(); }
	unsigned char* getFile(size_t id) { return &files[id][0]; }

private:
	//One for each "sub-file" found
	std::vector<std::unique_ptr<unsigned char[]>> files;
};
