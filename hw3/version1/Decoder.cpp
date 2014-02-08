#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include "HuffmanDecoder.h"
using namespace std;
// HuffmanDecoder
int main(int argc, char const *argv[])
{
	if(argc != 3)
	{
		cout << ":Usage:\n\t huffmanDecoding inputfile outputfile" << endl;
		return 0;
	}
	HuffmanDecoding hd(argv[1],argv[2]);
	hd.decode();
	return 0;
}