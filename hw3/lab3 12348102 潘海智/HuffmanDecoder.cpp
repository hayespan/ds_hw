#include "HuffmanDecoder.h"

HuffmanDecoding::HuffmanDecoding(string inDeFileName,string outDeFileName)
	:Huffman(inDeFileName,outDeFileName)
{}

//decoder
void HuffmanDecoding::decode()
{
	cout << "Begin decoding..." << endl;

	cout << "Opening file..." << endl;
	inFile_.open(inFileName_.c_str(),ios::in);
	inFile_.clear();

	cout << "Reading the number of bits..." << endl;
	inFile_ >> bitSize_;// read the number of bits
	cout << "Done!" << endl;

	cout << "Reading mapping-data..." << endl;
	getMap();// get mapping-relation and save it in nodeArray_
	cout << "Done!" << endl;

	cout << "Reading encoding-codes..." << endl;
	string str;
	getBits(str);// transform all bits to a string: str
	cout << "Done!" << endl;
	inFile_.close();

	cout << "Outputing decoding-result..." << endl;
	output(str);// output str to outputfile
	cout << "Done!" << endl;

	cout << "Decoding finished!" << endl;
}

// read mapping-data and save them in nodeArray_
void HuffmanDecoding::getMap()
{
	int n;
	inFile_ >> n;// 'n' is the number of mapping-pairs
	unsigned char c = 0;
	c = inFile_.get();//'c' gets 'endl' after 'n'
	c = inFile_.get();//'c' gets first unsigned char
	string t;
	while(n--)
	{
		inFile_ >> t;
		nodeArray_[c] -> code_ = t;
		c = inFile_.get();//'c' gets 'endl' after the previous unsigned char
		c = inFile_.get();//'c' gets the next unsigned char
	}
}

// transform all bits to a string(str)
void HuffmanDecoding::getBits(string &str)
{
	unsigned char c = inFile_.get();
	long long bitCount = 0;
	unsigned char t = 0x80;// t = 1000 0000
	while(!inFile_.eof())
	{
		bitCount += 8;
		if(bitCount > bitSize_)
		{
			for (int i = 0;i < bitSize_%8; ++i)
			{
				str += (c & t? '1':'0');
				c <<= 1;
			}
		}
		else
		{
			for (int i = 0; i < 8; ++i)
			{
				str += (c & t? '1':'0');
				c <<= 1;
			}
		}
		c = inFile_.get();
	}
}

// output str to outputfile
void HuffmanDecoding::output(string &str)
{
	string decodeStr;// decodeStr is a string to save decoding-result
	long long p = 0;// p is a current pointer to mark the beginning of str which needs to be decoded
	// use find(str,p) to find huffman code and then add the according unsigned char to str
	while(p != str.length())
	{
		for (int i = 0; i < MAXSIZE; ++i)
		{
			if(nodeArray_[i]->code_ != "")
			{
				if(str.find(nodeArray_[i]->code_,p) == p)
				{
					decodeStr += nodeArray_[i]->ch_;
					p += (nodeArray_[i]->code_).length();
					break;
				}
			}
		}
	}
	outFile_.open(outFilename_.c_str(),ios::out);
	outFile_.clear();
	outFile_ << decodeStr;// output str
	outFile_.close();
}
