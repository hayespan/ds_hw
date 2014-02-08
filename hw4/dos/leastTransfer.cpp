#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <list>
#include <queue>
#include <set>
#include <map>
#include <time.h>
#ifdef _WIN32
#include <Windows.h>
#endif
using namespace std;

// վ�������������������ýϴ����������Գ�վ�������ٸ�Ϊվ�����Խ�ʡ�ڴ�ռ䣬��˴�����Ϊ1454
const int MAX = 1454;

// ���νṹ�壬�������ࡢ������·������վ��
struct Line
{
	string busType_; // ��������
	vector<int> line_[2]; // line_[0]Ϊ�����ߣ�line_[1]Ϊ������
	Line(string busType = "")
		:busType_(busType)
	{}
	void push(int lineNum, int StationNum) // ��line_[lineNum]��ѹ��վ��node
	{
		line_[lineNum].push_back(StationNum);
	}
};

// վ��ṹ�壬����վ�����롰���롱��ʹ��unsigned char���ͣ�
struct Station
{
	int num_;// վ����
	unsigned char dist_; // վ�㵽��ʼվ����С�����롱�����������ٴ���
	Station(int num) :num_(num),dist_(0xff) {} // ��ʼ��dist_Ϊ���ֵ
	void setMaxDist()
	{
		dist_ = 0xff;
	}
};

// ����lessThan���������priority_queue
struct op
{
	bool operator()(Station * a, Station * b)
	{
		return a->dist_>b->dist_;
	}
};

set<string> nodeSet;// �������ϣ������ж�վ���Ƿ��ѱ�Ź���ʹ�ú������Խ�ʡ�ڴ�

map<string,int> getNum;// վ����-��ŵ�ӳ��ԣ����ڴ�վ��������Ӧ��ŵĸ�Ч����

std::vector<string> getName;// ���-վ���������������ڴӱ�ŵ�վ�����ĸ�Ч����

map<int,Line *> allLines;// ���й���·�ߵĴʵ䣬�Թ��������Ϊkey

vector<pair<int,int> > getLines[MAX];// ��¼ÿ��վ�����ڵ����г��Σ�����վ�ơ�����pair.firstΪ���α�ţ�pair.secondΪ�����б�־

unsigned char g[MAX][MAX];// ֱ����󣬵�a��b��ֱ������Ϊ1����ȫ�����������ۣ�������Ϊ255,ʹ��unsigned char���Ƚ�ʡ�ڴ�ռ�

Station * distArr[MAX]; // վ��ṹ������飬ʹ�ö�̬���뷽ʽ������dijkstra�㷨��priority_queue����������

bool known[MAX];// dijkstra�㷨����ļ�¼��ǰվ���Ƿ��ѱ���������Ϊ���߻�ͷ·

vector<int> parent[MAX];// ÿ��վ����ж�����ף�ʹ����������

int total = 0; // վ��������¼�����ݺ�������ֵ

int countWay = 1,step = 0; // ���ڼ������˷����ͻ��˲���

// ��ʼ��distArr��known��parent����
void init()
{
	for (int i = 0; i < total; ++i)
		distArr[i]->setMaxDist();
	memset(known,0,sizeof(known));
	for (int i = 0; i < total; ++i)
		parent[i].clear();
}

// �����ԡ�����ӡת��վ���ϵ���ݹ��ӡ��ʼվ-ת��վ-�յ�վ�ĸ��ӹ�ϵ
void showParent(int ori, int des)
{
	if(des == ori)
		return;
	else
	{
		cout << getName[des] << "'s parent(s): ";
		for (int i = 0; i < parent[des].size(); ++i)
		{
			cout << getName[parent[des][i]] << " ";
		}
		cout << "\n";
		for (int i = 0; i < parent[des].size(); ++i)
			showParent(ori,parent[des][i]);
	}
}

// �ݹ��ӡ���廻�˷�ʽ��Ϊinorder(list<int> &,int,int)������
void printHowToTransfer(list<int>::iterator beg, list<int>::iterator end)
{
	if(beg == end)
		return;
	else
	{
		list<int>::iterator nextBeg = ++beg;
		beg--;
		if(nextBeg == end)
		{
			cout << "\n";
			step = 0;
			return;
		}
		else
		{
			cout << "��" << ++step << "��:\n";
			for(int i = 0; i < getLines[*beg].size();i++)
			{
				int lineNum = getLines[*beg][i].first;
				int lineDirect = getLines[*beg][i].second;
				Line *t = allLines[lineNum];
				vector<int>::iterator begPos = find(t->line_[lineDirect].begin(),t->line_[lineDirect].end(),*beg);
				vector<int>::iterator nextBegPos = find(t->line_[lineDirect].begin(),t->line_[lineDirect].end(),*nextBeg);
				if(nextBegPos == t->line_[lineDirect].end())
					continue;
				if(begPos < nextBegPos)
				{
					cout << "     �ӡ�" << getName[*beg] << "��վ��� <" << lineNum << "�Ź�����(" 
						 <<  t->busType_ << ")-"; 
					if(lineDirect)
						cout << "������";
					else
						cout << "������";
					cout << "> ;�� " << nextBegPos-begPos-1 << " ��վ���" << getName[*nextBeg] << "��վ\n";
				}
			}
			printHowToTransfer(nextBeg,end);
		}
	}
}

// inorder�ݹ�����������������յ�վ���ݵ�����ʼվʱ����������route·����Ϊprint(int,int)������
void inorder(list<int> & route,int oriNum,int desNum)
{
	if(desNum == oriNum)
	{
		route.push_front(oriNum);
		printf("��������������������������������������������������������������������������������������������������������������������������������������������������������������\n");
		cout << "�� ����" << countWay++ << " ";
		int size = route.size(),count = 0;
		for (list<int>::iterator i = route.begin(); i != route.end(); ++i)
		{
			cout << getName[*i];
			if(count++ != size-1)
				cout << " �� "; 
		}
		cout << "\n\n";
		// �����ԡ�
		// for(list<int>::iterator i = route.begin();i!=route.end();i++)
		// {
		// 	cout << getName[*i] << " -> ";
		// }
		// cout << "\n";

		// ��ӡ���˷����ľ���
		printHowToTransfer(route.begin(),route.end());
		route.pop_front();
	}
	else
	{
		route.push_front(desNum);
		for (int i = 0; i < parent[desNum].size(); ++i)
			inorder(route,oriNum,parent[desNum][i]);
		route.pop_front();
	}
}

// ��ӡ�����Ķ��㺯��������һϵ���Ӻ���
void print(int oriNum,int desNum)
{
	list<int> route;
	inorder(route,oriNum,desNum);
}

// �����ԡ��򵥱����õ����˴�����������ûʹ�øú���
int getTransferTimes(int oriNum,int desNum)
{
	int transfertimes = 0;
	int current = desNum;
	while(current != oriNum)
	{
		transfertimes++;
		current = parent[current][0];
	}
	return --transfertimes;
}

// �����ԡ���ӡֱ�����
void printG()
{	
	for (int i = 0; i < total; ++i)
	{
		for (int j = 0; j < total; ++j)
		{
			cout << g[i][j]-'\0' << " ";
		}
		cout << "\n";
	}
}

// �����ԡ�����ӡdistArr
void printDistArr()
{
	for (int i = 0; i < total; ++i)
		cout << getName[distArr[i]->num_] << " 's dist: " << distArr[i]->dist_ << endl;
}

// ��ѯ���Ƚϼ򵥵�dijkstraʵ�֡���֮ͬ�����ڸ�ĸ����Ψһ��
void work(string & ori,string & des)
{
	int oriNum = getNum[ori];
	int desNum = getNum[des];
	distArr[oriNum] -> dist_ = 0;
	priority_queue<Station * &,vector<Station *>,op> pq;
	for (int i = 0; i < total; ++i)
		pq.push(distArr[i]);
	// cout << "��ʼ��distArr��\n";
	// printDistArr();
	while(!pq.empty())
	{
		Station *t = pq.top();
		pq.pop();
		int num = t->num_;
		int dist = t->dist_;
		known[num] = 1;
		// cout << "pop: " << getName[num] <<"!!!"<< distArr[num]->dist_<<"\n";
		if(num == desNum)
		{
			// cout << getName[num] << "!\n";
			break;
		}
		for (int i = 0; i < total; ++i)
		{
			if(!known[i])
			{
				int newDist = g[num][i]+dist;
				if(newDist < distArr[i]->dist_)
				{
					distArr[i]->dist_ = newDist;
					parent[i].clear();
					parent[i].push_back(num);
				}
				else if(newDist == distArr[i]->dist_)
					parent[i].push_back(num);
			}
		}
		priority_queue<Station * &,vector<Station *>,op> pqTemp;
		for (int i = 0; i < total; ++i)
			if(!known[i])
				pqTemp.push(distArr[i]);
		pq = pqTemp;// �о���һ��Ч�ʵ͡���
		// cout << "pop " << getName[num] << " 's distArr:\n";
		// printDistArr();
	}
	// ��Ϊû���������������ͨ�ԣ��Է���һ�����Ի��ǿ���һ�����������
	// ����޷�����ʼվͨ�����˵����յ�վ
	// ���յ�վ��parent����ʼվ����distΪ255����ʱ��ʾ�޷�����
	// ������Ҫ����һ�ξ������ͨ����ķ�ʽ���ݲ��迼�ǣ�
	if(parent[desNum][0] == oriNum && distArr[desNum]->dist_ == 255)
		cout <<"�޷��������Ҫ��ȡ������ʽŶ��\n\n";
	else
	{
		// ��ӡ���ٻ��˴�������ֱ����distArr[desNum]-1�õ�;
		// Ҳ��ʹ�ú��� getTransferTimes(ori,des) ������������Ҷ������һ��·���ó�
		cout << "�ף�Ҫ���ˡ�" <<  distArr[desNum]->dist_-1 << "����Ŷ��\n\n"
			 <<"���巽�����£�\n\n";

		// �����ԡ�����ӡվ��ĸ��ӹ�ϵ����ӡ���յ�վ����ʼվ��ȫ������תվ�㣩��
		// showParent(oriNum,desNum);

		// ��ӡ���巽��
		print(oriNum,desNum);
	}
}

// ��������ͣ��������ƽ̨
void clear(int t = 0)
{
	if(t > 0)
	{
		#ifdef _WIN32
			Sleep(1000*t);
		#else
			sleep(t);
		#endif
	}
	#ifdef _WIN32
		system("cls");
	#else
		system("clear");
	#endif
}

// ������
int main(int argc, char const *argv[])
{
	cout << "��ʼ¼��վ������...\n";
	clock_t timeBeg = clock();
	// ��������¼��
	fstream data("alllines.txt");
	if(data.fail())
	{
		cout << "ȱʧ�ļ�alllines.txt��2����˳�...\n";
		clear(2);
		return 0;
	}
	while(1)
	{
		int busId;// ���α��
		string busType;// ��������
		string stopName;// վ����
		data >> busId;
		if(busId == -1)
			break;
		data >> busType;
		Line *t = new Line(busType);// �����³���
		for (int i = 0; i < 2; ++i)
		{
			int temp;
			data >> temp;
			for (int j = 0; j < temp; ++j)// ��ȡ������·��
			{
				data >> stopName;
				if(!nodeSet.count(stopName))
				{
					nodeSet.insert(stopName);
					getNum[stopName] = total;
					getName.push_back(stopName);
					total++;	
				}
				int num = getNum[stopName];
				t->push(i,num);
				getLines[num].push_back(make_pair<int,int>(busId,i));
			}
		}
		allLines[busId] = t;
	}
	nodeSet.clear();// ����
	data.close();

	// �����ԡ���ȷ����Ϣ¼�����󣬲��Ե�֪����1454��վ��,293����������586����·�����������У�
	// cout << "The size of getNum is " << getNum.size() << "\n"
	// 	 << "The size of getName is " << getName.size() << "\n"
	// 	 << "The size of allLines is " << allLines.size() << "\n";
	// for (map<string,int>::iterator i = getNum.begin();i != getNum.end();i++)
	// 	cout << i->first << " : " << i->second << "\n";
	// for (int i=0;i<getName.size();i++)
	// 	cout << i << " : " << getName[i] << "\n";
	// for (int i = 0; i < total; ++i)
	// {
	// 	cout << getName[i] << ":\n";
	// 	for (int j = 0; j < getLines[i].size(); ++j)
	// 	{
	// 		cout << "(" << getLines[i].at(j).first << "," << getLines[i].at(j).second << ")" <<" ";
	// 	}
	// 	cout << "\n";
	// }
	// for(map<int,Line *>::iterator i = allLines.begin();i!=allLines.end();i++)
	// {
	// 	cout << i->first << "\n"
	// 		 << i->second->busType_<<"\n";
	// 	for (int j = 0; j < 2; ++j)
	// 	{
	// 		int size = i->second->line_[j].size();
	// 		cout << size << "\n";
	// 		for (int k = 0; k < size; ++k)
	// 			cout << getName.at(i->second->line_[j][k]) << "\n";
	// 	}
	// }

	// ��ʼ��ֱ�������distArr
	memset(g,255,sizeof(g));
	for (int i = 0; i < total; ++i)
	{
		for (vector<pair<int,int> >::iterator j = getLines[i].begin();j != getLines[i].end();j++)
		{
			int busId = j->first;
			int lineDirect = j->second;
			Line *t = allLines[busId];
			vector<int>::iterator fs = find(t->line_[lineDirect].begin(),t->line_[lineDirect].end(),i);
			for (vector<int>::iterator k = fs; k != t->line_[lineDirect].end(); ++k)
				g[i][*k] = 1;
		}
	}
	for (int i = 0; i < total; ++i)
		distArr[i] = new Station(i);

	// �����ԡ������ֱ������Ƿ���ȷ
	// printG();

	clock_t timeEnd = clock();// ¼�봦�����ݼ�ʱ
	cout << "վ�����ݴ�����ɣ���ʱ " << ((double)(timeEnd - timeBeg))/CLOCKS_PER_SEC << " ��\n";
	clear(2);

	// ��ѯ
	while(1)
	{
		// ������ʼվ���յ�վ
		string ori,des;
		printf("��������ʼվ:\n");
		cin >> ori;
		printf("�������յ�վ:\n");
		cin >> des;
		if(ori == des || getNum.find(ori) == getNum.end() || getNum.find(des) == getNum.end())
		{
			clear();
			cout << "���������룡\n";// ���������Ч��ʾ��������
			clear(1);
			continue;
		}
		clear();
		cout << "\n��ʼվ��" << ori 
			 << "\n�յ�վ��" << des 
			 << "\n\n";

		// ��ʼ��dijkstra�㷨����������ݣ�ÿ�ν���ѭ������ԭdistArr��known��parent����
		init();

		// ��ʼ��������Ϊ1
		countWay = 1;
		timeBeg = clock();// ��ѯ��ӡ������ʱ

		// dijkstra�㷨�����в�ѯ����ӡת�˽��
		work(ori,des);

		timeEnd = clock();
		cout << "�˴β�ѯ��ʱ " << ((double)(timeEnd - timeBeg))/CLOCKS_PER_SEC << " ��\n\n";

		// �����˳�����
		char quitSignal;
		printf("�˳�? y/n\n");
		cin >> quitSignal;
		if(quitSignal == 'y')
			break;
		clear();
	}
	clear();
	return 0;
}
