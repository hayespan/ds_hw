#include <iostream>
#include <queue>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include "Random.h"
#include "Window.h"
#include "Client.h"
using namespace std;

int BEGIN_TIME = 0;
int END_TIME = 10;
int UP_SER_TIME = 3;
int POISSION_MEAN = 5;
int mode = 0;

int main(int argc, char const *argv[])
{
	Random r(false);
	srand((unsigned int)time(0));

	cout << "choose queue mode:(1 for multi-queue and 0 for single-queue)" << endl;
	cin >> mode;
	Window::setMode(mode);

	cout << "input begin-time:"<<endl;
	cin >> BEGIN_TIME;

	cout << "input end-time:"<<endl;
	cin >> END_TIME;

	cout << "input upmost service time per client:"<<endl;
	cin >> UP_SER_TIME;

	cout << "input the mean of poisson: "<<endl;
	cin >> POISSION_MEAN;

	Window w[5];
	int totalWaitingTime = 0;
	for(int curTime = BEGIN_TIME;curTime < END_TIME;curTime++)
	{
		system("clear");

		//不同模式下，顾客入队
		int cliNum = r.poisson(POISSION_MEAN);
		for(int i=0;i<cliNum;i++)
		{
			Client * t = new Client(curTime,rand()%UP_SER_TIME+1);
			if(Window::getMode())
			{
				int min = w[0].getDivQueueSize(),k = 0;
				if(!w[0].getSpare())
					min++;
				for(int j=1;j<5;j++)
				{
					int temp = w[j].getDivQueueSize();
					if(!w[j].getSpare())
						temp++;
					if(temp < min)
					{
						min = temp;
						k = j;
					}
				}
				w[k].enDivQueue(t);
			}
			else
			{
				Window::enHeadQueue(t);
			}
		}

		// 窗口更新当前工作状态
		for (int i = 0; i < 5; ++i)
			if(!w[i].getSpare() && curTime - w[i].getSerCli()->getDequeueTime() == w[i].getSerCli()->getSerTime())
				w[i].setSpare(1);

		// 不同模式下，顾客出队并接受服务
		if(Window::getMode())
		{
			for (int i = 0; i < 5; ++i)
				if(w[i].getSpare() && w[i].getDivQueueSize() > 0)
				{
					w[i].setSerCli(w[i].deDivQueue());
					w[i].getSerCli()->setDequeueTime(curTime);
					totalWaitingTime += w[i].getSerCli()->getWaitingTime();
					w[i].setSpare(0);
				}
		}
		else
		{
			for (int i = 0; i < 5; ++i)
			{
				if(w[i].getSpare())
				{
					if(Window::getHeadQueueSize() > 0)
					{
						w[i].setSerCli(Window::deHeadQueue());
						w[i].getSerCli()->setDequeueTime(curTime);
						totalWaitingTime += w[i].getSerCli()->getWaitingTime();
						w[i].setSpare(0);
					}
					else
						break;
				}
			}
		}

		// 打印动态
		cout << "____________________________________________________________"<<endl;
		cout << setw(35) << right << "current time: " << curTime << endl;
		for (int i = 0; i < 5; ++i)
		{
			cout << setw(8) << left << " Window "
				 << setw(6) << left << w[i].getId();
			if(w[i].getSpare())
				cout << setw(26) << left << "Spare"
					 << setw(13) << left << "Waiting Time "
					 << setw(7) << left << "0";
			else
				cout << setw(16) << left << "serving client "
					 << setw(10) << left << w[i].getSerCli()->getIndex()
					 << setw(13) << left << "Waiting Time "
					 << setw(7) << left << w[i].getSerCli()->getWaitingTime();
			cout << endl;
		}
		int waitingCliNum = 0;
		if(Window::getMode())
			for (int i = 0; i < 5; ++i)
				waitingCliNum += w[i].getDivQueueSize();
		else
			waitingCliNum = Window::getHeadQueueSize();
		cout << "\nThe number of waiting-clients: " << waitingCliNum << endl; 
		cout << "____________________________________________________________"<<endl;

		sleep(1);
	}

	// 排队系统关闭，不再接收客户
	system("clear");
	cout << "QueueSystem shutdown!" << endl;
	sleep(2);

	// 继续服务完队列和窗口中的顾客
	for(int curTime = END_TIME;;curTime++)
	{
		system("clear");

		// 窗口更新当前工作状态
		for (int i = 0; i < 5; ++i)
			if(!w[i].getSpare() && curTime - w[i].getSerCli()->getDequeueTime() == w[i].getSerCli()->getSerTime())
				w[i].setSpare(1);

		// 不同模式下，顾客出队并接受服务
		if(Window::getMode())
		{
			for (int i = 0; i < 5; ++i)
				if(w[i].getSpare() && w[i].getDivQueueSize() > 0)
				{
					w[i].setSerCli(w[i].deDivQueue());
					w[i].getSerCli()->setDequeueTime(curTime);
					totalWaitingTime += w[i].getSerCli()->getWaitingTime();
					w[i].setSpare(0);
				}
		}
		else
		{
			for (int i = 0; i < 5; ++i)
			{
				if(w[i].getSpare())
				{
					if(Window::getHeadQueueSize() > 0)
					{
						w[i].setSerCli(Window::deHeadQueue());
						w[i].getSerCli()->setDequeueTime(curTime);
						totalWaitingTime += w[i].getSerCli()->getWaitingTime();
						w[i].setSpare(0);
					}
					else
						break;
				}
			}
		}

		//检查是否完成全部工作，是的话退出循环
		bool over = 1;
		for (int i = 0; i < 5; ++i)
		{
			if(!w[i].getSpare())
			{
				over = 0;
				break;
			}
		}
		if(over)
		{
			system("clear");
			cout << "Finish!" << endl;
			sleep(2);
			break;
		}

		// 打印动态
		cout << "____________________________________________________________"<<endl;
		cout << setw(35) << right << "current time: " << curTime << endl;
		for (int i = 0; i < 5; ++i)
		{
			cout << setw(8) << left << " Window "
				 << setw(6) << left << w[i].getId();
			if(w[i].getSpare())
				cout << setw(26) << left << "Spare"
					 << setw(13) << left << "Waiting Time "
					 << setw(7) << left << "0";
			else
				cout << setw(16) << left << "serving client "
					 << setw(10) << left << w[i].getSerCli()->getIndex()
					 << setw(13) << left << "Waiting Time "
					 << setw(7) << left << w[i].getSerCli()->getWaitingTime();
			cout << endl;
		}
		int waitingCliNum = 0;
		if(Window::getMode())
			for (int i = 0; i < 5; ++i)
				waitingCliNum += w[i].getDivQueueSize();
		else
			waitingCliNum = Window::getHeadQueueSize();
		cout << "\nThe number of waiting-clients: " << waitingCliNum << endl; 
		cout << "____________________________________________________________"<<endl;

		sleep(1);
	}

	system("clear");

	// 输出服务总人数，总等待时长和平均等待时常
	cout << "statistic:" << endl;
	for (int i = 0; i < 5; ++i)
	{
		cout << setw(10) << left << " Window "
			 << setw(4) << left << w[i].getId()
			 << " has served " << w[i].getSerCliNum() << " clients." << endl;
	}
	cout << "The number of served-clients: " << Client::getIndexes() << endl
		 << "The total waiting-time is: " << totalWaitingTime << endl
		 << "The average waiting-time is: " << totalWaitingTime*1.0/Client::getIndexes() << endl;
	
	return 0;
}