#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include "HuffmanNode.h"
using namespace std;

//Huffman class is designed to encode, so in fact it's a encoder
class Huffman
{
public:
	Huffman(string inFileName, string outFilename);

	// encoder
	void encode();

	// find frequency of all ascii chars and save them in nodeArray_
	// then create priority-queue
	void createPq();

	// make use of priority-queue to build a Huffman-Tree
	void createHuffmanTree();

	// calculate Huffman-Code
	void calculateHuffmanCode();

	//save encoding-result to outputfile
	void saveToFile();

	// restore
	~Huffman();

protected:
	static const int MAXSIZE = 256;
	HuffmanNode * nodeArray_[MAXSIZE];
    fstream inFile_, outFile_;
	HuffmanNode * root_;
	int size_;// the number of ascii chars which have non-empty code_
	long long bitSize_;// the number of bits
	string inFileName_,outFilename_;

	// a Compare function for priority-queue
	// a Compare function for priority-queue
	class Compare
	{
	public:
		bool operator()(HuffmanNode * c1, HuffmanNode * c2) const
		{
			return (*c1).freq_ > (*c2).freq_;
		}
	};

 	// a priority-queue to make building Huffman-Tree more convenient
 	priority_queue<HuffmanNode *, std::vector<HuffmanNode *>, Compare > pq_;

 	// find frequency of ascii chars and save them in nodeArray_
 	void createNodeArray();

 	// calculate Huffman-Code by recursion 
	void calculateHuffmanCodeFunc(HuffmanNode* &root, int t, string &temp);

	// so as to debug, just ignore it
	void inorder_recursive_print(HuffmanNode* root);

	// save mapping-data to outputfile
	void saveMap();

	// save the number of bits to outputfile
	void saveBitSize();

};

#endif
