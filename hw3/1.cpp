#include <iostream>
#include <fstream>
#include <string>
#include <queue>
using namespace std;

// all nodes in Huffman-Tree are HuffmanNode
// the difference between internal node and external node are:
// 1.internal node's ch_ is -1, external node's ch_ is between 0~255
// 2.internal node's code_ is "", external node's code_ is not "" 
struct HuffmanNode
{
	HuffmanNode(char ch = 0, int freq = 0, HuffmanNode * left = 0, HuffmanNode * right = 0, HuffmanNode * parent = 0)
		:ch_(ch),freq_(freq),code_(""),left_(left),right_(right),parent_(parent)
	{}
	char ch_;
	int freq_;
	string code_;
	HuffmanNode * left_,*right_,*parent_;
};

//Huffman class is designed to encode, so in fact it's a encoder
class Huffman
{
public:
	Huffman(string inFileName, string outFilename)
		:root_(0),inFileName_(inFileName),outFilename_(outFilename),size_(0),bitSize_(0)
	{
		for(int i = 0; i < MAXSIZE; ++i)
			nodeArray_[i] = new HuffmanNode((char)(i),0);
	}
	// encoder
	void encode()
	{
		cout << "Begin encoding..." << endl;

		cout << "Open file..." << endl;
 		inFile_.open(inFileName_.c_str(),ios::in);

 		cout << "Calculating frequency of all ascii chars..." << endl;
 		createNodeArray();
 		cout << "Done!" << endl;
 		inFile_.close();

		for (int i = 0; i < MAXSIZE; ++i)
		{
			cout << i << " char: " << nodeArray_[i]->ch_ << " , " << nodeArray_[i]->freq_ << endl;
		}

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
	void createPq()
	{
		for (int i = 0; i < MAXSIZE; ++i)
			if(nodeArray_[i]->freq_)
				pq_.push(nodeArray_[i]);
	}
	// make use of priority-queue to build a Huffman-Tree
	void createHuffmanTree()
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
	void calculateHuffmanCode()
	{
		string temp;
		calculateHuffmanCodeFunc(root_,-1,temp);
	}
	//save encoding-result to outputfile
	void saveToFile()
	{
		outFile_.open(outFilename_.c_str(),ios::out);
		saveBitSize();// output the number of bits
		saveMap();// output mapping-data
		string str;
		unsigned char c = inFile_.get();
		inFile_.open(inFileName_.c_str(),ios::in);
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
	~Huffman()
	{
		for(int i=0;i<MAXSIZE;i++)
			delete nodeArray_[i];
	}
protected:
	static const int MAXSIZE = 256;
	HuffmanNode * nodeArray_[MAXSIZE];
    fstream inFile_, outFile_;
	HuffmanNode * root_;
	int size_;// the number of ascii chars which have non-empty code_
	long long bitSize_;// the number of bits
	string inFileName_,outFilename_;
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
 	void createNodeArray()
 	{
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
	void calculateHuffmanCodeFunc(HuffmanNode* &root, int t, string &temp)
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
	void inorder_recursive_print(HuffmanNode* root)
	{
	    if(root != 0)
	    {
	        inorder_recursive_print(root -> left_);
	        cout << root->ch_ << " : " << root->freq_ << " : " << root->code_ <<endl;
	        inorder_recursive_print(root -> right_);
	    }
	}
	// save mapping-data to outputfile
	void saveMap()
	{
		outFile_ << size_ << endl;
		for(int i=0;i<MAXSIZE;i++)
			if(nodeArray_[i]->freq_!=0)
				outFile_ << (unsigned char)i << nodeArray_[i]->code_<< endl;
		outFile_ << endl;
	}
	// save the number of bits to outputfile
	void saveBitSize()
	{
		outFile_ << bitSize_ << " ";
	}
};
const int Huffman::MAXSIZE;

// extended from Huffman class and it is a HuffmanDecoder
class HuffmanDecoding : public Huffman
{
public:
	HuffmanDecoding(string inDeFileName,string outDeFileName)
		:Huffman(inDeFileName,outDeFileName)
	{}
	void decode()
	{
		cout << "Begin decoding..." << endl;

		cout << "Opening file..." << endl;
		inFile_.open(inFileName_.c_str(),ios::in);
		
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

		cout << "Outputing decoding-result..." << endl;
		output(str);// output str to outputfile
		inFile_.close();
		cout << "Done!" << endl;

		cout << "Decoding finished!" << endl;
	}
private:
	// read mapping-data and save them in nodeArray_ 
	void getMap()
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
	void getBits(string &str)
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
	void output(string &str)
	{
		string decodeStr;// decodeStr is a string to save decoding-result
		int p = 0;// p is a current pointer to mark the beginning of str which needs to be decoded
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
		cout << decodeStr << endl;
		outFile_.open(outFilename_.c_str(),ios::out);
		outFile_ << decodeStr;// output str
		outFile_.close();
	}
};

// HuffmanEncoder
/*int main(int argc, char const *argv[])
{
	if(argc != 3)
	{
		cout << ":Usage:\n\t huffmancoding inputfile outputfile" << endl;
		return 0;
	}
	Huffman h(argv[1],argv[2]);
	h.encode();
	return 0;
}*/

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
