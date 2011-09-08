#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include "zlib/zlib.h"

using namespace std;


/////////////////////////////////////////////////////////////////////////
//NOTE: A few things aobut this file. First, it's just a temporary test of
//      how FF7 stores bin files using gzip. I matched their version of gzip
//      to zlib-1.1.3, and it decompresses byte-accurately with that version
//      of zlib's "gzip"-style functions. It does NOT compress again to byte-perfect
//      accuracy; the following indices are off:
//      file1, [8], and 8 bytes short.
//      file2, [8], and 8 bytes short.
//      In both cases, "02" was expected but "00" was computed.
//      According to the specification, this is the "XFL" flag, which should be set to:
//      2 - compressor used maximum compression,
//      4 - compressor used fastest algorithm
//      ...if CM==8 (it does). So... why is it zero?
//      After reading through the code, it seems that XFL is set to zero in zlib, and
//      it is simply ignored when reading in the file. I think the point is that
//      the gzip header contains a lot of stuff that file information that isn't needed
//      when dealing with raw data. Now.... why are 8 bytes missing from an otherwise-perfect file?
//
//      Ok, figured it out: the CRC and total written length are appended as a footer to
//      the gzip file after it's closed(). Since we don't close temporary files, that never happens.
//      Wrote some test code and everything matches now (except the _2_, but we can hack that in manually
//      if we want to). If we add a tiny fix for that, our code will be RTT-compatible. Yay!
/////////////////////////////////////////////////////////////////////////

namespace {
//const unsigned int CHUNK_SIZE = 1024 * 256; //256k chunk size is efficient, and can easily be spared
}


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
		std::cout <<"Unexpected EOF (1)\n";
		return false;
	}

	//The next word specifies the size of the decompressed file
	res.decompressedSize = ReadWord(combined);
	if (combined.eof()) {
		if (res.gzipCompressedBlocks!=0) {
			std::cout <<"Unexpected EOF (2)\n";
		}
		return false;
	}

	//The last word in the header represent a counter, which is of unknown use
	//NOTE: I am guessing that the counter means "number of files left in this chunk"?
	res.unknownCounter = ReadWord(combined);
	if (combined.eof()) {
		std::cout <<"Unexpected EOF (3)\n";
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

	//Now, test round-trip compression by zipping it up again!
	unique_ptr<unsigned char[]> zippedData2 = unique_ptr<unsigned char[]>(new unsigned char[header.gzipCompressedBlocks]);
	FILE* tmpFile = tmpfile();
	gzFile anotherFD = gzdopen(fileno(tmpFile), "wb9");
	int read = gzwrite(anotherFD, reinterpret_cast<char*>(&result[0]), header.decompressedSize);
	if (read<0 || (unsigned int)read!=header.decompressedSize) {
		std::cout <<"Error writing to gzipped file: " <<read <<"\n";
		return false;
	}
	read = gzflush(anotherFD, Z_FINISH);
	rewind(tmpFile);
	read = fread(reinterpret_cast<char*>(&zippedData2[0]), 1, header.gzipCompressedBlocks, tmpFile);
	if ((unsigned int)read!=header.gzipCompressedBlocks) {
		std::cout <<"File underrun: " <<read <<" instead of " <<header.gzipCompressedBlocks <<"\n";
		//return false;
	}

	//Compare
	for (size_t i=0; i<header.gzipCompressedBlocks; i++) {
		if (zippedData[i] != zippedData2[i]) {
			std::cout <<"File content mismtach on ID: " <<i <<"\n";
			//return false;
		}
		if (i==header.gzipCompressedBlocks-1) {
			std::cout <<"File  " <<(i+1) <<" compresses ok.\n";
		}
	}



	return true;
}



//The goal:    To test using zlib to extract/recombine FF7's crazy .bin setup.
//The problem: It requires an obscure version of zlib.
int main()
{
	//Check for files.
	ifstream combined("co.bin", ios::binary|ios::in);
	ifstream firstRes("co.bin0", ios::binary|ios::in);
	ifstream secondRes("co.bin1", ios::binary|ios::in);
	if (!combined.is_open() || !firstRes.is_open() || !secondRes.is_open()) {
		std::cout <<"Can't find input file(s)\n";
		return 1;
	}

	//Each sub-file in this chunk starts with a header:
	//NOTE:
	BinHeader header;
	vector<unique_ptr<unsigned char[]>> files;
	vector<size_t> fileSizes;
	for(;;) {
		//Base case
		if (!ReadHeader(combined, header)) {
			break;
		}

		//Read the file
		std::cout <<"Found sub-file: " <<header.gzipCompressedBlocks <<" blocks, " <<header.decompressedSize <<" decompressed, " <<header.unknownCounter <<" counter (unknown)" <<"\n";
		files.push_back(unique_ptr<unsigned char[]>(new unsigned char[header.decompressedSize]));
		fileSizes.push_back(header.decompressedSize);
		if (!ReadSubFile(combined, header, files.back())) {
			return 1;
		}
	}
	combined.close();


	//Compare them
	for (size_t i=0; i<files.size(); i++) {
		//Retrieve
		ifstream& origStream = i==0 ? firstRes : secondRes;
		unsigned char* gen = &files[i][0];
		size_t sz = fileSizes[i];

		//Read
		unique_ptr<unsigned char[]> orig = unique_ptr<unsigned char[]>(new unsigned char[sz]);
		origStream.read(reinterpret_cast<char*>(&orig[0]), sz);
		if (origStream.eof()) {
			std::cout <<"File size mismatch\n";
			return 1;
		}

		//Compare
		for (size_t x=0; x<sz; x++) {
			if (gen[x] != orig[x]) {
				std::cout <<"File content mismtach on ID: " <<x <<"\n";
				return 1;
			}

			if (x==sz-1) {
				std::cout <<"File  " <<(i+1) <<" decompresses ok.\n";
			}
		}
	}


	//Done
	std::cout <<"Done\n";
	return 0;
}


