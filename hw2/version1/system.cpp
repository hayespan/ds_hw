#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include "RANDOM.H"
#define WINNUM 5
#define OPT 9
#define CLT 14
const double RANCUS = 5;
const double RANTIME = 2;

using namespace std;

class customer{
	public:
		customer(int arrTime,int serTime,int num)
		  :num(num),arrTime(arrTime),leftTime(0),serTime(serTime),dequeueTime(0){
		}
		int getNum()
		{
			return num;
		}
		int getSerTime()
		{
			return serTime;
		}
		
		int getArrTime()
		{
			return arrTime;
		}
		void setDequeueTime(int dequeueTime)
		{
			this->dequeueTime = dequeueTime;
		}
		int getDequeueTime()
		{
			return dequeueTime;
		}
		void setLeftTime(int leftTime)
		{
			this->leftTime = leftTime;
		}
		
	private:
		int num;
		int arrTime;
		int leftTime;
		int serTime;
		int dequeueTime;
};

int totalCusNum = 0;
int totalSerTime = 0;
int totalWaitTime = 0;
int curTime = 0;

vector<customer*> servedCus;

class windows
{
	public:
	    windows(int id):id(id),servedCusNum(0),onWork(0){
	    }
	    int getId()
	    {
	    	return id;
	    }
	    
	    void setId(int id)
	    {
	    	this->id = id;
	    }
	    bool isOnWork()
	    {
	    	return onWork;
	    }
	    void getACus(customer* p)
	    {
	    	cus = p;
	    	onWork = true;
	    }
	    int getServedCusNum()
	    {
	    	return servedCusNum;
	    }
	    void addSerNum()
	    {
	    	servedCusNum++;
	    }
	    
	    void work1()
	    {
	    	if(isOnWork())
	    	{
	    		if(cus->getSerTime() == curTime-cus->getDequeueTime())
	    		{
	    		   cus->setLeftTime(curTime);
	    		   servedCusNum++;
				   servedCus.push_back(cus);
				   totalSerTime += cus->getSerTime();
				   setSpare();
	    	    }
	    	}
	    }
	    
	    void addToWinWait(customer* p)
	    {
	    	winWait.push(p);
	    }
	    
	    queue<customer*>& getWinWait()
	    {
	    	return winWait;
	    }
	    
	    customer* getCus()
		{
			return cus;
		} 
	    void setSpare()
	    {
	    	onWork = false;
	    }
	private:
		int id;
		int servedCusNum;
		bool onWork;
		customer* cus;
		queue<customer*> winWait;
};

void open(vector<windows*> &ws)
{
	for(int i=0; i<WINNUM; i++)
	{
		ws.push_back(new windows(i+1));
    }
}

int findLeastWin(vector<windows*> &ws)
{
	int tem1 = 0; 
	int tem2 = ws[0]->getWinWait().size();
	for(int i=0;i<WINNUM; i++)
	{
		if( (!ws[i]->isOnWork()) &&  ws[i]->getWinWait().size()==0 )
		{
		    tem1 = i;
		    break;
	    }
	    else if( ws[i]->isOnWork() && ws[i]->getWinWait().size() < tem2)
	    {
	    	tem1 = i;
		    break;
	    }
	}
	return tem1;
}

void bankWork0(vector<windows*> &ws,queue<customer*> &line)
{
    customer* tem;
    while(!line.empty())
    {
		tem = line.front();
		line.pop();
        int s = findLeastWin(ws);
		ws[s]->addToWinWait(tem);
    }
    for(int i=0; i<WINNUM; i++)
    {
    	ws[i]->work1();
    	if(!ws[i]->isOnWork())
    	{
		    if(!(ws[i]->getWinWait().empty()))
		    {
    	        tem = ws[i]->getWinWait().front();
    	        ws[i]->getWinWait().pop();
    	    }
    	    else continue;
    	    tem->setDequeueTime(curTime);
		    int te = tem->getDequeueTime() - tem->getArrTime(); 
		    totalWaitTime += te;  
    	    ws[i]->getACus(tem);  
        }
        
    }
}

void bankWork1(vector<windows*> &ws,queue<customer*> &line)
{
	for(int i=0; i<WINNUM; i++)
	{
		ws[i]->work1();
		if(!ws[i]->isOnWork())   
		{
    		customer* tem;
			if(!line.empty())
				tem = line.front();
			else break;
			line.pop();
			tem->setDequeueTime(curTime);
			int te = tem->getDequeueTime() - tem->getArrTime(); 
			totalWaitTime += te;  
    		ws[i]->getACus(tem);
    		continue;
    	}
	}
}

void completeWork(vector<windows*> &ws)
{
	for(int i=0; i<WINNUM; i++)
	{
		if(ws[i]->isOnWork())
		{
		    ws[i]->getCus()->setLeftTime(ws[i]->getCus()->getArrTime()+ws[i]->getCus()->getSerTime());
	        ws[i]->addSerNum();
	        servedCus.push_back(ws[i]->getCus());
	   	    totalSerTime += ws[i]->getCus()->getSerTime();
		    ws[i]->setSpare();
		}
		else
		    continue;
	}
}

void printWin0(vector<windows*> ws)
{
   for(int i=0; i<WINNUM; i++)
   {
       if(ws[i]->isOnWork())
          cout << "Windows #" << ws[i]->getId() << " is serving. The number of waiting customer in this window is "
		     << ws[i]->getWinWait().size() << "." << " The Serving customer is " << "$" << ws[i]->getCus()->getNum() << endl;
       else
          cout << "Windows #" << ws[i]->getId() << " is empty." << endl;
   }
}

void printWin1(vector<windows*> ws)
{
   for(int i=0; i<ws.size(); i++)
   {
       if(ws[i]->isOnWork())
          cout << "Windows #" << ws[i]->getId() << " is serving. The Serving customer is " 
	        << "$" << ws[i]->getCus()->getNum() << endl;
       else
          cout << "Windows #" << ws[i]->getId() << " is empty." << endl;
   }
}

void printFinal(vector<windows*> &ws,int totalNum, double aveTime)
{
	cout << "The total number of served customs is " << servedCus.size() << endl;
	cout << "The number of unserved customes is " << totalCusNum - servedCus.size() << endl;
    cout << "The number of served customs in each window are below:" << endl;
    for(int i=0; i<ws.size(); i++)
        cout << "Windows #" << ws[i]->getId() << ":  " << ws[i]->getServedCusNum() << endl;
    cout << "The average waiting time is " << aveTime << endl;
}

int main()
{
	cout << "The bank is opened! It will work from Time " << OPT << " to Time " << CLT << " !" << endl;
	cout << "Please determine whether to queue one line(1 for Yes and 0 for No)" << endl;
	bool mode;
	cin >> mode;
	
	Random r(false);

	totalCusNum = 0;
	queue<customer*> wait;
	vector<windows*> win;
	open(win);
	if(mode){
	    for(curTime = OPT; curTime<CLT; curTime++)
	    {
		    int randCusNum = r.poisson(RANCUS);
		    cout << "At time " << curTime << ", " << randCusNum << " customers come!" << endl;
		    for(int i=totalCusNum+1; i<=totalCusNum+randCusNum; i++)
	          wait.push(new customer(curTime,r.poisson(RANTIME),i));
	        totalCusNum += randCusNum;
	        bankWork1(win,wait);
	        cout << "********************Information********************" << endl;
	        cout << "The number of waiting customer is " << wait.size() << endl;
	        cout << "**********************WinInfo**********************" << endl;
	    
	        printWin1(win);
	        cout << endl;  
        }
        completeWork(win);
		double averageTime = totalWaitTime * 1.0 / servedCus.size();
		printFinal(win,servedCus.size(),averageTime);
		cout << "The bank is closed !" << endl;
	}
	else
	{
		for(curTime = OPT; curTime<CLT; curTime++)
		{
		    int randCusNum = r.poisson(RANCUS);
			cout << "At time " << curTime << ", " << randCusNum << " customers come!" << endl;
			for(int i=totalCusNum+1; i<=totalCusNum+randCusNum; i++)
	          wait.push(new customer(curTime,r.poisson(RANTIME),i));
	        totalCusNum += randCusNum;	
	        
	        bankWork0(win,wait);
	        int waitCusTotal = 0;
	        for(int i=0; i<WINNUM; i++)
	        {
	        	waitCusTotal += win[i]->getWinWait().size();
	        }
	        cout << "********************Information********************" << endl;
	        cout << "The number of waiting customer is " << waitCusTotal << endl;
	        cout << "**********************WinInfo**********************" << endl;
	    
	        printWin0(win);
	        cout << endl;  
		}
		completeWork(win);
		double averageTime = totalWaitTime * 1.0 / servedCus.size();
		printFinal(win,servedCus.size(),averageTime);
		cout << "The bank is closed !" << endl;
	}    
    return 0;
}

