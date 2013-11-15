#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <map>
#include <set>
#include <iomanip>
#include <algorithm>
using namespace std;

struct op;
//check whether main arguments are legal
bool checkArgc(int & argc, char const *argv[]);	
//cut a line into words and store them in vector v
void cutWord(string & line, vector<string> & v);
//store each word that appear in article with its line number set
void saveData(int lineNum, const vector<string> & v, map<string, set<int> > & data);
//output result in *.ind
void print(const map<string,set<int> > & data, const set<string> & keys, ofstream & out);

int main(int argc, char const *argv[])
{
	if(!checkArgc(argc,argv))
		return 0;

	vector<pair<string,vector<int> > > data;
	string line;
	int lineNum = 1;

	ifstream in,keysin;
	ofstream out;

	//open files
	in.open(argv[1]);
	keysin.open(argv[2]);
	out.open(argv[3]);

	//store key words in vector "keys"
	string temp;
	while(keysin >> temp)
	{
		data.push_back(make_pair<string,vector<int> >(temp,vector<int>(0)));
	}
	sort(data.begin(),data.end(),op);

	//read article in lines
	while(getline(in,line))
	{
		vector<string> wordInLine;
		cutWord(line,wordInLine);
		saveData(lineNum,wordInLine,data);
		lineNum++;
	}


	//output result
	print(data,out);

	//close files
	in.close();
	keysin.close();
	out.close();

	return 0;
}

//less than fuction object for sort
struct op
{
	bool operator< (pair<string,vector<int> > a, pair<string,vector<int> > b)
	{
		return a.first<b.first;
	}
};

//check whether main arguments are legal
bool checkArgc(int & argc, char const *argv[])
{
	if(argc == 4)
	{
		ifstream inTemp[3];
		inTemp[0].open(argv[1]);
		inTemp[1].open(argv[2]);
		inTemp[2].open(argv[3]);

		if(!inTemp[0])
		{
			cout << "error!\nfile: " << argv[1] << " does not exist!\n";
			return 0;
		}
		if(!inTemp[1])
		{
			cout << "error!\nfile: " << argv[2] << " does not exist!\n";
			return 0;
		}
		if(!inTemp[2])
		{
			cout << "error!\nfile: " << argv[3] << " does not exist!\n";
			return 0;	
		}
		inTemp[0].close();
		inTemp[1].close();
		inTemp[2].close();

		return 1;
	}
	else
		cout << "error!\nyou should run index in this way: >index inputfile keys outputfile\n";
	return 0;
}

//cut a line into words and store them in vector v
void cutWord(string & line, vector<string> & v)
{
	line = " "+line+" ";
	std::vector<int> vi;
	for(int i=1;i<line.length();i++)
		if(( !isalpha(line[i-1]) && isalpha(line[i]) ) || ( !isalpha(line[i]) && isalpha(line[i-1]) ))
			vi.push_back(i);
	for(int i=0;i<vi.size();i += 2)
		v.push_back(line.substr(vi.at(i),vi.at(i+1)-vi.at(i)));
}

//store each word that appear in article with its line number set
void saveData(int lineNum, const vector<string> & wordInLine, vector<pair<string,vector<int> > > & data)
{
	for(int i=0;i<data.size();i++)
		if(find(wordInLine.begin(),wordInLine.end(),data.at(i).first) != wordInLine.end())
			data[i].second.push_back(lineNum);
}

//output result in *.ind
void print(const vector<pair<string,vector<int> > > & data, ofstream & out)
{
	typedef vector<pair<string,vector<int> > >::const_iterator cvpit;
	for(cvpit i=data.begin();i!=data.end();i++)
	{
		out << setw(8) <<left << i->first;
		out << '\t';
		for(vector<int>::const_iterator j = i->second.begin();j != i->second.end();j++)
		{
			out << *j;
			if(j != --i->second.end())
				out << ",";
		}
		out << endl;
	}
}