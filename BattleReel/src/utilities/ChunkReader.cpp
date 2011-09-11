#include "ChunkReader.hpp"

#include "zlib/zlib.h"

using std::ios;
using std::string;
using std::ifstream;
using std::unique_ptr;


//Some helpers
namespace {
struct BinHeader {
	unsigned int gzipCompressedBlocks;
	unsigned int decompressedSize;
	unsigned int unknownCounter;
};

unsigned int ReadWord(ifstream& combined)
{
	unsigned char buffer[2] = {0,0};
	combined.read(reinterpret_cast<char*>(&buffer[0]), 2);
	return (buffer[1]<<8) | buffer[0];
}

bool ReadHeader(ifstream& combined, BinHeader& res)
{
	//The first word specifies the number of blocks gzip should decompress
	// Each file ends with an "empty file", where this value is of size 00
	res.gzipCompressedBlocks = ReadWord(combined);
	if (combined.eof()) {
		std::cout <<"Unexpected EOF\n";
		return false;
	}

	//The next word specifies the size of the decompressed file
	res.decompressedSize = ReadWord(combined);
	if (combined.eof()) {
		if (res.gzipCompressedBlocks!=0) {
			std::cout <<"Unexpected EOF\n";
		}
		return false;
	}

	//The last word in the header represent a counter, which is of unknown use
	//NOTE: I am guessing that the counter means "number of files left in this chunk"?
	res.unknownCounter = ReadWord(combined);
	if (combined.eof()) {
		std::cout <<"Unexpected EOF\n";
		return false;
	}

	return true;
}

bool ReadSubFile(ifstream& combined, const BinHeader& header, unique_ptr<unsigned char[]>& result)
{
	//Ensure we've got something to work with
	if (header.gzipCompressedBlocks==0 || header.decompressedSize==0) {
		std::cout <<"Sub-file is zero blocks or zero bytes\n";
		return false;
	}

	//Read all the blocks that gzip will expect. Save these into a temporary file.
	unique_ptr<unsigned char[]> zippedData = unique_ptr<unsigned char[]>(new unsigned char[header.gzipCompressedBlocks]);
	{
	FILE * tmpFile = tmpfile();
	{
		combined.read(reinterpret_cast<char*>(&zippedData[0]), header.gzipCompressedBlocks);
		if (combined.eof()) {
			std::cout <<"Sub-file is shorter than expected.\n";
			return false;
		}

		//Save it, rewind
		fwrite(reinterpret_cast<char*>(&zippedData[0]), header.gzipCompressedBlocks, 1, tmpFile);
		rewind(tmpFile);
	}

	//Now read it with the gzip() functions
	gzFile anotherFD = gzdopen(fileno(tmpFile), "rb");
	int read = gzread(anotherFD, reinterpret_cast<char*>(&result[0]), header.decompressedSize);
	if (read<0 || (unsigned int)read!=header.decompressedSize) {
		std::cout <<"Error reading from gzipped file: " <<read <<"\n";
		return false;
	}
	fclose(tmpFile);
	}

	return true;
}

}



bool ChunkReader::loadFile(const string& path)
{
	//Remove and delete all char* arrays.
	files.clear();

	//Try to open the file
	ifstream combined(path, ios::binary|ios::in);
	if (!combined.is_open()) {
		std::cout <<"Can't open file: " <<path <<"\n";
		return false;
	}

	//Read each single file
	BinHeader header;
	for(;;) {
		//Base case
		if (!ReadHeader(combined, header)) {
			break;
		}

		//Read the file
		files.push_back(unique_ptr<unsigned char[]>(new unsigned char[header.decompressedSize]));
		if (!ReadSubFile(combined, header, files.back())) {
			std::cout <<"Error reading sub-file.\n";
			return false;
		}
	}
	combined.close();

	//Success
	return true;
}




