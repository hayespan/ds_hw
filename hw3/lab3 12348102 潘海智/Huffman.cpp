#include "Huffman.h"

Huffman::Huffman(string inFileName, string outFilename)
	:root_(0),inFileName_(inFileName),outFilename_(outFilename),size_(0),bitSize_(0)
{
	for(int i = 0; i < MAXSIZE; ++i)
		nodeArray_[i] = new HuffmanNode((char)(i),0);
}

// encoder
void Huffman::encode()
{
	cout << "Begin encoding..." << endl;

	cout << "Open file..." << endl;
		inFile_.open(inFileName_.c_str(),ios::in);

		cout << "Calculating frequency of all ascii chars..." << endl;
		createNodeArray();
		cout << "Done!" << endl;
		inFile_.close();

	cout << "Creating Priority-Queue..." << endl;
	createPq();
	cout << "Done!" << endl;

	cout << "Creating Huffman-Tree..." << endl;
	createHuffmanTree();
	cout << "Done!" << endl;

	cout << "Calculating Huffman-Code..." << endl;
	calculateHuffmanCode();
	cout << "Done!" << endl;

	cout << "Saving to outputfile..." << endl;
	saveToFile();
	cout << "Done!" << endl;

	cout << "Ecoding finished!" << endl;
}

// find frequency of all ascii chars and save them in nodeArray_
// then create priority-queue
void Huffman::createPq()
{
	for (int i = 0; i < MAXSIZE; ++i)
		if(nodeArray_[i]->freq_)
			pq_.push(nodeArray_[i]);
}

// make use of priority-queue to build a Huffman-Tree
void Huffman::createHuffmanTree()
{
	if(pq_.empty())
		root_ = 0;
	else if(pq_.size() == 1)
	{
		root_ = new HuffmanNode(-1,pq_.top()->freq_,pq_.top());
		pq_.pop();
		root_->left_->parent_ = root_;
	}
	else
	{
		while(pq_.size()!=1)
		{
			HuffmanNode * p1 = pq_.top();
			pq_.pop();
			HuffmanNode * p2 = pq_.top();
			pq_.pop();
			HuffmanNode * p = new HuffmanNode(-1,p1->freq_+p2->freq_,p1,p2);
			p->left_->parent_ = p;
			p->right_->parent_ = p;
			pq_.push(p);
		}
		root_ = pq_.top();
		pq_.pop();
	}
}

// calculate Huffman-Code
void Huffman::calculateHuffmanCode()
{
	string temp;
	calculateHuffmanCodeFunc(root_,-1,temp);
}

//save encoding-result to outputfile
void Huffman::saveToFile()
{
	outFile_.open(outFilename_.c_str(),ios::out);
	outFile_.clear();
	saveBitSize();// output the number of bits
	saveMap();// output mapping-data
	string str;
	inFile_.open(inFileName_.c_str(),ios::in);
	inFile_.clear();
	unsigned char c = inFile_.get();
	while(!inFile_.eof())
	{
		str += nodeArray_[c]->code_;
		c = inFile_.get();
	}
	inFile_.close();
	//output encoding-codes, the unit is byte
	unsigned char buffer = 0;
	int k = 0;
	long long bitCount = 0;
	for(int i=0;i<str.length();i++)
	{
		if(str[i] == '1')
			buffer++;
		k++;
		if(k == 8)
		{
			outFile_ << buffer;
			bitCount += 8;
			buffer = 0;
			k = 0;
			continue;
		}
		buffer <<= 1;
	}
	if(bitCount != bitSize_)
	{
		for (int i = 0; i < 7-bitSize_+bitCount; ++i)
			buffer <<= 1;
		outFile_ << buffer;
	}
	outFile_.close();
}

// restore
Huffman::~Huffman()
{
	for(int i=0;i<MAXSIZE;i++)
		delete nodeArray_[i];
}

// find frequency of ascii chars and save them in nodeArray_
void Huffman::createNodeArray()
{
	inFile_.clear();
	unsigned char c = inFile_.get();
	while(!inFile_.eof())
	{
		nodeArray_[c]->freq_++;
		c = inFile_.get();
	}
	for(int i=0;i<MAXSIZE;i++)
		if(nodeArray_[i]->freq_ != 0)
			size_++;
}

// calculate Huffman-Code by recursion
void Huffman::calculateHuffmanCodeFunc(HuffmanNode* &root, int t, string &temp)
{
    if(root != 0)
    {
    	if(t > -1)
        	temp += (char)('0'+t);
        if(root->ch_ != -1)
        {
        	root -> code_ = temp;
        	bitSize_ += root->freq_ * root->code_.length();
        }
        else
        {
		    calculateHuffmanCodeFunc(root -> left_,0,temp);
		    calculateHuffmanCodeFunc(root -> right_,1,temp);
        }
        if(t > -1)
        	temp.erase(temp.length()-1);
    }
}

// so as to debug, just ignore it
void Huffman::inorder_recursive_print(HuffmanNode* root)
{
    if(root != 0)
    {
        inorder_recursive_print(root -> left_);
        cout << root->ch_ << " : " << root->freq_ << " : " << root->code_ <<endl;
        inorder_recursive_print(root -> right_);
    }
}

// save mapping-data to outputfile
void Huffman::saveMap()
{
	outFile_ << size_ << endl;
	for(int i=0;i<MAXSIZE;i++)
		if(nodeArray_[i]->freq_!=0)
			outFile_ << (unsigned char)i << " " << nodeArray_[i]->code_<< endl;
	outFile_ << endl;
}

// save the number of bits to outputfile
void Huffman::saveBitSize()
{
	outFile_ << bitSize_ << " ";
}
const int Huffman::MAXSIZE;
