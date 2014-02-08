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

// 站点的最大数量，可先设置较大数量，测试出站点数后再改为站点数以节省内存空间，如此次数据为1454
const int MAX = 1454;

// 车次结构体，包含种类、上下行路线所有站点
struct Line
{
	string busType_; // 车次种类
	vector<int> line_[2]; // line_[0]为上行线，line_[1]为下行线
	Line(string busType = "")
		:busType_(busType)
	{}
	void push(int lineNum, int StationNum) // 往line_[lineNum]中压入站点node
	{
		line_[lineNum].push_back(StationNum);
	}
};

// 站点结构体，包含站点编号与“距离”（使用unsigned char类型）
struct Station
{
	int num_;// 站点编号
	unsigned char dist_; // 站点到起始站的最小“距离”，即换乘最少代价
	Station(int num) :num_(num),dist_(0xff) {} // 初始化dist_为最大值
	void setMaxDist()
	{
		dist_ = 0xff;
	}
};

// 重载lessThan运算符用于priority_queue
struct op
{
	bool operator()(Station * a, Station * b)
	{
		return a->dist_>b->dist_;
	}
};

set<string> nodeSet;// 辅助集合，用于判断站点是否已编号过，使用后会清空以节省内存

map<string,int> getNum;// 站点名-编号的映射对，用于从站点名到对应编号的高效索引

std::vector<string> getName;// 编号-站点名的向量，用于从编号到站点名的高效索引

map<int,Line *> allLines;// 所有公交路线的词典，以公交车编号为key

vector<pair<int,int> > getLines[MAX];// 记录每个站点所在的所有车次（即“站牌”），pair.first为车次编号，pair.second为上下行标志

unsigned char g[MAX][MAX];// 直达矩阵，当a到b可直达则置为1（完全忽略其他代价），否则为255,使用unsigned char，比较省内存空间

Station * distArr[MAX]; // 站点结构体的数组，使用动态申请方式，用于dijkstra算法的priority_queue，便于排序

bool known[MAX];// dijkstra算法所需的记录当前站点是否已被坐过，因为不走回头路

vector<int> parent[MAX];// 每个站点可有多个父亲，使用向量保存

int total = 0; // 站点总量，录入数据后会更新数值

int countWay = 1,step = 0; // 用于计数换乘方案和换乘步骤

// 初始化distArr、known和parent数组
void init()
{
	for (int i = 0; i < total; ++i)
		distArr[i]->setMaxDist();
	memset(known,0,sizeof(known));
	for (int i = 0; i < total; ++i)
		parent[i].clear();
}

// 【测试】，打印转乘站点关系，递归打印起始站-转乘站-终点站的父子关系
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

// 递归打印具体换乘方式，为inorder(list<int> &,int,int)所调用
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
			cout << "第" << ++step << "步:\n";
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
					cout << "     从【" << getName[*beg] << "】站搭乘 <" << lineNum << "号公交车(" 
						 <<  t->busType_ << ")-"; 
					if(lineDirect)
						cout << "下行线";
					else
						cout << "上行线";
					cout << "> 途经 " << nextBegPos-begPos-1 << " 个站到达【" << getName[*nextBeg] << "】站\n";
				}
			}
			printHowToTransfer(nextBeg,end);
		}
	}
}

// inorder递归遍历树方法，当从终点站回溯到达起始站时，生成链表route路径。为print(int,int)所调用
void inorder(list<int> & route,int oriNum,int desNum)
{
	if(desNum == oriNum)
	{
		route.push_front(oriNum);
		printf("———————————————————————————————————————————————————————————————————————————————\n");
		cout << "● 方案" << countWay++ << " ";
		int size = route.size(),count = 0;
		for (list<int>::iterator i = route.begin(); i != route.end(); ++i)
		{
			cout << getName[*i];
			if(count++ != size-1)
				cout << " → "; 
		}
		cout << "\n\n";
		// 【测试】
		// for(list<int>::iterator i = route.begin();i!=route.end();i++)
		// {
		// 	cout << getName[*i] << " -> ";
		// }
		// cout << "\n";

		// 打印换乘方案的具体
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

// 打印方案的顶层函数，调用一系列子函数
void print(int oriNum,int desNum)
{
	list<int> route;
	inorder(route,oriNum,desNum);
}

// 【测试】简单遍历得到换乘次数，代码中没使用该函数
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

// 【测试】打印直达矩阵
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

// 【测试】，打印distArr
void printDistArr()
{
	for (int i = 0; i < total; ++i)
		cout << getName[distArr[i]->num_] << " 's dist: " << distArr[i]->dist_ << endl;
}

// 查询，比较简单的dijkstra实现。不同之处在于父母数不唯一。
void work(string & ori,string & des)
{
	int oriNum = getNum[ori];
	int desNum = getNum[des];
	distArr[oriNum] -> dist_ = 0;
	priority_queue<Station * &,vector<Station *>,op> pq;
	for (int i = 0; i < total; ++i)
		pq.push(distArr[i]);
	// cout << "初始的distArr：\n";
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
		pq = pqTemp;// 感觉这一步效率低。。
		// cout << "pop " << getName[num] << " 's distArr:\n";
		// printDistArr();
	}
	// 因为没测试任意两点的连通性，以防万一，所以还是考虑一下这种情况。
	// 如果无法从起始站通过换乘到达终点站
	// 则终点站的parent是起始站并且dist为255，此时提示无法到达
	// （可能要步行一段距离或者通过别的方式，暂不予考虑）
	if(parent[desNum][0] == oriNum && distArr[desNum]->dist_ == 255)
		cout <<"无法到达，可能要采取其它方式哦～\n\n";
	else
	{
		// 打印最少换乘次数，可直接由distArr[desNum]-1得到;
		// 也可使用函数 getTransferTimes(ori,des) 遍历树根到树叶的其中一条路径得出
		cout << "亲，要换乘【" <<  distArr[desNum]->dist_-1 << "】次哦～\n\n"
			 <<"具体方案如下：\n\n";

		// 【测试】，打印站点的父子关系（打印从终点站到起始站的全部可能转站点））
		// showParent(oriNum,desNum);

		// 打印具体方案
		print(oriNum,desNum);
	}
}

// 清屏、暂停函数，跨平台
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

// 主函数
int main(int argc, char const *argv[])
{
	cout << "开始录入站点数据...\n";
	clock_t timeBeg = clock();
	// 公交数据录入
	fstream data("alllines.txt");
	if(data.fail())
	{
		cout << "缺失文件alllines.txt，2秒后退出...\n";
		clear(2);
		return 0;
	}
	while(1)
	{
		int busId;// 车次编号
		string busType;// 车次种类
		string stopName;// 站点名
		data >> busId;
		if(busId == -1)
			break;
		data >> busType;
		Line *t = new Line(busType);// 申请新车次
		for (int i = 0; i < 2; ++i)
		{
			int temp;
			data >> temp;
			for (int j = 0; j < temp; ++j)// 读取上下行路径
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
	nodeSet.clear();// 回收
	data.close();

	// 【测试】，确认信息录入无误，测试得知共有1454个站点,293辆公交车，586条线路（区别上下行）
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

	// 初始化直达矩阵与distArr
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

	// 【测试】，检查直达矩阵是否正确
	// printG();

	clock_t timeEnd = clock();// 录入处理数据计时
	cout << "站点数据处理完成！耗时 " << ((double)(timeEnd - timeBeg))/CLOCKS_PER_SEC << " 秒\n";
	clear(2);

	// 查询
	while(1)
	{
		// 输入起始站与终点站
		string ori,des;
		printf("请输入起始站:\n");
		cin >> ori;
		printf("请输入终点站:\n");
		cin >> des;
		if(ori == des || getNum.find(ori) == getNum.end() || getNum.find(des) == getNum.end())
		{
			clear();
			cout << "请重新输入！\n";// 如果输入无效提示重新输入
			clear(1);
			continue;
		}
		clear();
		cout << "\n起始站：" << ori 
			 << "\n终点站：" << des 
			 << "\n\n";

		// 初始化dijkstra算法所需相关数据，每次进入循环都还原distArr、known和parent数组
		init();

		// 初始化方案数为1
		countWay = 1;
		timeBeg = clock();// 查询打印方案计时

		// dijkstra算法，进行查询，打印转乘结果
		work(ori,des);

		timeEnd = clock();
		cout << "此次查询耗时 " << ((double)(timeEnd - timeBeg))/CLOCKS_PER_SEC << " 秒\n\n";

		// 控制退出程序
		char quitSignal;
		printf("退出? y/n\n");
		cin >> quitSignal;
		if(quitSignal == 'y')
			break;
		clear();
	}
	clear();
	return 0;
}
