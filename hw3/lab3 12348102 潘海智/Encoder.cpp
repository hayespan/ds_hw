#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include "Huffman.h"
using namespace std;
// HuffmanEncoder
int main(int argc, char const *argv[])
{
	if(argc != 3)
	{
		cout << ":Usage:\n\t huffmancoding inputfile outputfile" << endl;
		return 0;
	}
	Huffman h(argv[1],argv[2]);
	h.encode();
	return 0;
}