#ifndef HUFFMANDECODER_H
#define HUFFMANDECODER_H

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include "Huffman.h"
#include "HuffmanNode.h"
using namespace std;

// extended from Huffman class and it is a HuffmanDecoder
class HuffmanDecoding : public Huffman
{
public:
	HuffmanDecoding(string inDeFileName,string outDeFileName);
	void decode();
private:
	// read mapping-data and save them in nodeArray_ 
	void getMap();

	// transform all bits to a string(str)
	void getBits(string &str);

	// output str to outputfile
	void output(string &str);
};

#endif
