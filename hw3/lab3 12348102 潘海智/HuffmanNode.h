#ifndef HUFFMANNODE_H
#define HUFFMANNODE_H

#include <iostream>
#include <string>
using namespace std;
// all nodes in Huffman-Tree are HuffmanNode
// the difference between internal node and external node are:
// 1.internal node's ch_ is -1, external node's ch_ is between 0~255
// 2.internal node's code_ is "", external node's code_ is not "" 
struct HuffmanNode
{
	HuffmanNode(char ch = 0, int freq = 0, HuffmanNode * left = 0, HuffmanNode * right = 0, HuffmanNode * parent = 0);
	char ch_;
	int freq_;
	string code_;
	HuffmanNode * left_,*right_,*parent_;
};

#endif
