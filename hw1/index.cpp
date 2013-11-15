#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <map>
#include <set>
#include <iomanip>
using namespace std;

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

	map<string,set<int> > data;
	string line;
	int lineNum = 1;

	ifstream in,keysin;
	ofstream out;

	//open files
	in.open(argv[1]);
	keysin.open(argv[2]);
	out.open(argv[3]);

	//read article in lines
	while(getline(in,line))
	{
		vector<string> v;
		cutWord(line,v);
		saveData(lineNum,v,data);
		lineNum++;
	}

	//store key words in vector "keys"
	string temp;
	set<string> keys;
	while(keysin >> temp)
		keys.insert(temp);

	//output result
	print(data,keys,out);

	//close files
	in.close();
	keysin.close();
	out.close();

	return 0;
}

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
void saveData(int lineNum, const vector<string> & v, map<string, set<int> > & data)
{
	for (int i = 0; i < v.size(); ++i)
		data[v.at(i)].insert(lineNum);	
}

//output result in *.ind
void print(const map<string,set<int> > & data, const set<string> & keys, ofstream & out)
{
	typedef map<string,set<int> >::const_iterator cmit;
	cmit it = data.end();
	for(set<string>::const_iterator i=keys.begin();i != keys.end();i++)
	{
		cmit p = data.find(*i);
		if(p != it)
		{
			out << setw(8) << left << p->first;
			out << '\t';
			for(set<int>::const_iterator k = (p->second).begin(); k != (p->second).end(); k++)
			{
				out << *k;
				if(k != --(p->second).end())
					out << ",";
			}
			out << endl;
		}
	}
}