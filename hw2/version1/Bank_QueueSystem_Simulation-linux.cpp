#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <string>
#include <cstdlib>
#include <queue>
#include "RANDOM.H"
using namespace std;

//forward declaration
class Customer;//
class Device;
class Window;
class QueueSystem;
void pushServedCusVect(Customer * );
void pushUnservedCusVect(Customer * );

//current time, unit-time == 1 minute
int I = 0;
//vector for customers who had been served and left the bank
vector<Customer *> servedCusVect;
//vector for customers who had enqueued but not been served
vector<Customer *> unservedCusVect;
//determine whether to follow the tracks of customers or not
int track = 0;
//determine whether to print dynamic information in cmd
int printDynamic = 0;
//dialog ofstream object
ofstream dialog;


class Customer
{
public:
	Customer(int enqueueTime, int serTime)
		:enqueueTime_(enqueueTime),serTime_(serTime),num_(++totalNum_),dequeueTime_(-1),leftBankTime_(-1)
	{}
	int enqueueTime()
	{
		return enqueueTime_;
	}
	int dequeueTime()
	{
		return dequeueTime_;
	}
	void setDequeueTime(int dequeueTime)
	{
		dequeueTime_ = dequeueTime;
	}
	int cusNum()
	{
		return num_;
	}
	void setLeftBankTime(int leftBankTime)
	{
		leftBankTime_ = leftBankTime;
	}
	int leftBankTime()
	{
		return leftBankTime_;
	}
	int cusSerTime()
	{
		return serTime_;
	}
	static int enqueueNum()
	{
		return totalNum_;
	}
private:
	int num_;//index of customer
	int enqueueTime_;//enqueue time
	int dequeueTime_;//dequeue time
	int leftBankTime_;//after service and left bank time
	int serTime_;//service time
	static int totalNum_;//total number of customers
};
int Customer::totalNum_ = 0;

//push served-customers to servedCusVect
void pushServedCusVect(Customer * c)
{
	if(track)
		dialog<<"customer "<<c->cusNum()<<" left the bank at time "<<c->leftBankTime()
			<<" and it cost him "<< c->leftBankTime()-c->enqueueTime() <<" minutes in total" <<endl;
	servedCusVect.push_back(c);
}
//push unserved-customers to unservedCusVect
void pushUnservedCusVect(Customer * c)
{
	if(track)
		dialog<<"unserved customer "<<c->cusNum()<<" left the bank at time "<<c->leftBankTime()
			<<" and it cost him "<< c->leftBankTime()-c->enqueueTime() <<" minutes in total" <<endl;
	unservedCusVect.push_back(c);
}

//service-windows
class Window
{
public:
	Window()
		:id_(++winNum_),cus_(0),servedCusNum_(0),totalTime_(0),onWork_(0)
	{}
	int getId() //get the id of according service-window
	{
		return id_;
	}
	//check whether the work is done, if was, ask the customer to leave and increase totalTime
	void winWork()
	{
		if(isOnWork())
		{
			if(cus_->cusSerTime() == I-cus_->dequeueTime())
			{
				cus_->setLeftBankTime(I);
				totalTime_ += cus_->cusSerTime();
				if(track)
					dialog<<"customer "<<cus_->cusNum()<<" left window "<<id_<<" at time "<<I
						<<" and his service-time is "<<cus_->cusSerTime() <<endl;
				pushServedCusVect(cus_);
				setSpare();
			}
		}
	}
	//when queue system arranges a customer to the window, call enWork()
	void enWork(Customer * p)
	{
		if(track)
			dialog << "customer "<<p->cusNum()<<" got to window "<<id_<<" at time "<<I<<endl;
		onWork_ = 1;			
		cus_ = p;
		servedCusNum_++;
	}
	bool isOnWork()
	{
		return onWork_;
	}
	void setSpare()
	{
		onWork_ = 0;
		cus_ = 0;
	}
	//print real-time dynamic information of the service-window
	//including "serving who" or "spare" 
	void printSer()
	{
		dialog << setw(16)<<left<<"*";
		if(printDynamic)
			cout << setw(16)<<left<<"*";
		dialog << "Window " << setw(2) << left << id_<< setw(6) << left <<" ";
		if(printDynamic)
			cout << "Window " << setw(2) << left << id_<< setw(6) << left <<" ";
		if(isOnWork())
		{
			dialog << setw(18) << left << "serving customer " << setw(4) << left << cus_->cusNum();
			if(printDynamic)
				cout << setw(18) << left << "serving customer " << setw(4) << left << cus_->cusNum();
		}
		else
		{
			dialog << setw(22) << left << "spare";
			if(printDynamic)
				cout << setw(22) << left << "spare";
		}
		dialog << setw(18) << right << "*" << endl;
		if(printDynamic)
			cout << setw(18) << right << "*" << endl;
	}
	//when the bank should be shutdown while there are still some customers being served, 
	//keep on serving them until the work is done
	void closeWin()
	{
		if(isOnWork())
		{
			cus_->setLeftBankTime(cus_->dequeueTime()+cus_->cusSerTime());
			totalTime_ += cus_->cusSerTime();
			if(track)
				dialog<<"customer "<<cus_->cusNum()<<" left window "<<id_<<" at time "<<cus_->leftBankTime()
					<<" and his service-time is "<<cus_->cusSerTime() <<endl;
			pushServedCusVect(cus_);
			setSpare();
		}
	}
	int servedCusNum()
	{
		return servedCusNum_;
	}
	int servedTotalTime()
	{
		return totalTime_;
	}
private:
	int id_;
	Customer * cus_;
	int servedCusNum_;
	int totalTime_;
	bool onWork_;
	static int winNum_;
};
int Window::winNum_ = 0;

//this is the Ticketer, in fact it's a STL::queue* vector
class Device
{
public:
	Device()
		:q_(),devId_(++devTotalNum_)
	{}
	int size()
	{
		return q_.size();
	}
	bool empty()
	{
		return q_.empty();
	}
	//when a customer wants to enqueue, presses the button and he will get a number(index)
	void press(Customer * c)
	{
		if(track)
			dialog<<"customer "<<c->cusNum()<<" enqueued in queue-"<<devId_<<" at time "<< I <<endl;
		q_.push(c);
	}
	//
	Customer * pop()
	{
		Customer * t = q_.front();
		q_.pop();
		t->setDequeueTime(I);//when customer dequeues, set dequeue-time for him
		//check if the customer is unserved, if was, print 'unserved' to distinguish itself from the served
		int check = t->dequeueTime() - t->leftBankTime();
		if(track)
		{
			if(check == 0)
				dialog << "unserved ";
			dialog<<"customer "<<t->cusNum()<<" dequeued at time "<< I 
				<<" and he has been waiting for "<<I-t->enqueueTime()<<" minutes" << endl;
		}
		return t;
	}
	Customer * front()
	{
		return q_.front();
	}
private:
	queue<Customer *> q_;
	int devId_;
	static int devTotalNum_;
};
int Device::devTotalNum_ = 0;

//yes, it's your own queue system
class QueueSystem 
{
public:
	QueueSystem(int openTime, int closeTime, int winNum,int mode)
		:openTime_(openTime),closeTime_(closeTime),mode_(mode)
	{
		for(int i=0;i<winNum;i++)
			winv_.push_back(new Window());

		//in different mode, the number of queues is different
		if(mode_==1)// there is only 1 queue
			dev_.push_back(new Device());
		else //the number of queues is according to the number of service-windows
			for(int i=0;i<winNum;i++)
				dev_.push_back(new Device());
	}
	//different modes make queue-style different, so pressing the button means there are 2 ways to enqueue
	void pressDevice(Customer * c)
	{
		if(mode_ == 1)
			dev_[0]->press(c);//only 1 queue so you have no choice
		else
		{
			//choose the shortest queue
			int t = dev_[0]->size();
			int k=0;
			for(int i=1;i<dev_.size();i++)
				if(t > dev_[i]->size())
				{
					t = dev_[i]->size();
					k = i;
				}
			dev_[k]->press(c);
		}
	}

	// QueueSystem's mission is to arrange enqueued-customers to spare windows to get services 
	void work()
	{
		//call winWork()
		//service-windows may be spare or serving someone, so ask them to refresh status 
		for(int i=0;i<winv_.size();i++)
			winv_[i]->winWork();
		
		//mode-1 arrange-style
		if(mode_ == 1)
		{
			for(int i=0;i<winv_.size();i++)
				if(!winv_[i]->isOnWork())
				{
					if(!dev_[0]->empty())
					{
						winv_[i]->enWork(dev_[0]->pop());
						continue;
					}
					else
						break;
				}
		}
		//mode-2 arrange-style
		else
			for(int i=0;i<winv_.size();i++)
				if(!winv_[i]->isOnWork())
					if(!dev_[i]->empty())
						winv_[i]->enWork(dev_[i]->pop());
	}

	// print real-time dynamic information in screen
	// print present-time, the number of served-customers and the number of waiting-customers
	// print all windows' information: 'serving someone' or 'spare'
	void printWinDynInfo()
	{
		dialog << "*******************************report**********************************"<<endl;
		if(printDynamic)
			cout << "*******************************report**********************************"<<endl;
		dialog<<setw(10)<<left<<"*"
			<<setw(6)<<left<<"Time: " // present-time
			<<setw(10)<<left<<I
			<<setw(8)<<left<<"served: " // the number of served-customers
			<<setw(10)<<left<<servedCusVect.size()
			<<setw(9)<<left<<"waiting: "; // the number of waiting-customers in queue(s)
		if(printDynamic)
			cout<<setw(10)<<left<<"*"
				<<setw(6)<<left<<"Time: " // present-time
				<<setw(10)<<left<<I
				<<setw(8)<<left<<"served: " // the number of served-customers
				<<setw(10)<<left<<servedCusVect.size()
				<<setw(9)<<left<<"waiting: "; // the number of waiting-customers in queue(s)
			if(mode_ == 1) // mode 1 
			{
				dialog << setw(10) << left << dev_[0]->size();
				if(printDynamic)
					cout << setw(10) << left << dev_[0]->size();
			}
			else //mode 2
			{
				int t=0;
				for(int i=0;i<dev_.size();i++)
					t += dev_[i]->size();
				dialog << setw(10) << left << t;
				if(printDynamic)
					cout << setw(10) << left << t;
			}
			dialog << setw(8) << right << "*" << endl;
			if(printDynamic)
				cout << setw(8) << right << "*" << endl;
		dialog << "*                                                                     *" << endl;
		if(printDynamic)
			cout << "*                                                                     *" << endl;

		//call window's printSer() to print their work information: serving someone or spare
		for(int i=0;i<winv_.size();i++)
			winv_[i]->printSer();

		dialog << "***********************************************************************" << endl;
		if(printDynamic)
			cout << "***********************************************************************" << endl;
	}

	//print all windows' served-customer-number and total serving-time in final dialog for conclusion
	//for example, 'Window 1:  23  100'
	//             'Window 2:  20  95'
	void printWinCon()
	{
		dialog << setw(20) << left << "window-id"
			 << setw(20) << left << "served-number"
			 << setw(20) << left << "serving-time\n"<<endl;
		for(int i=0;i<winv_.size();i++)
			dialog << setw(8) << left << "Window "
				 << setw(12)<< left << winv_[i]->getId()
				 << setw(20)<< left << winv_[i]->servedCusNum()
				 << setw(20)<< left << winv_[i]->servedTotalTime()<<endl;
	}
	//when the bank is time to shutdown, no customer can enqueue 
	//order all service-windows not to shutdown until all work is done
	void closeQueueSystem()
	{
		//mode 1 
		if(mode_ == 1)
			while(!dev_[0]->empty())
			{
				Customer *p = dev_[0]->front();
				p->setLeftBankTime(I);
				dev_[0]->pop();
				pushUnservedCusVect(p);
			}
		else //mode 2
			for(int i=0;i<dev_.size();i++)
				while(!dev_[i]->empty())
				{
					Customer *p = dev_[i]->front();
					p->setLeftBankTime(I);
					dev_[i]->pop();
					pushUnservedCusVect(p);
				}	
	}
	//finish the work that is not done when bank shutdowns
	void finishWork()
	{
		for(int i=0;i<winv_.size();i++)
			winv_[i]->closeWin();		
	}

private:
	int openTime_;//bank open-time
	int closeTime_;//bank close-time
	int mode_;//choose a queue style for the queue system, one-queue or multiqueues
	vector<Window *> winv_;//build as many service-windows as you like
	vector<Device *> dev_;//of course you can equip as many Ticketers as you like
};

//main fuction test this bank QueueSystem simulation
int main(int argc, char const *argv[])
{
	//all data will be saved in 'DIALOG.txt'
	dialog.open("DIALOG.txt");

	int bankOpenTime,bankCloseTime,bankWinNum,cusNumPer,cusCost,mode;//"mode" means different queue-style 
	cout << "You should first build a queue system\nPlease input open-time of it:\n(a non-negative number x(minute))\n";
	cin >> bankOpenTime;
	cout << "Please input close-time of it: \n(bigger than open-time, y(minute))\n";
	cin >> bankCloseTime;
	cout << "Please input the number of service-windows:\n";
	cin >> bankWinNum;
	cout << "Please input the current number of customers per minute(the \"mean\" for Poisson _distribution):\n";
	cin >> cusNumPer;
	cout << "Please input the upper limit service-time each customer may spend:\n";
	cin >> cusCost;
	cout << "Please input the queue-mode for the queue system:\n(1 or 2)\n1: Only 1 queue\n2: The number of queues is according to the number of service-windows\n";
	cin >> mode;
	cout << "Follow the tracks of all customers and print them in dialog?\n(input 1 if you wanna, otherwise 0)\n";
	cin >> track;
	cout << "Do you wanna see dynamic information in cmd?\n(It cost you about y-x seconds to see the whole progress)\n(input 1 if you wanna, otherwise 0)\n";
	cin >> printDynamic;

	//build your queue system!
	QueueSystem queueSys(bankOpenTime,bankCloseTime,bankWinNum,mode);

	srand((unsigned int)time(0));//use time(0) as the seed for srand()
	Random r(false);// data in different testcase are different

	//the time 'I' is going
	for (I = bankOpenTime; I < bankCloseTime; ++I)
	{
		if(printDynamic)
			system("clear");

		// use random fuction to generate the number of customers that may enqueue per minute
		// (1)Common random:
		// int randCusNum = rand()%cusNumPer;
		// (2)Poisson distribution: 
		int randCusNum = r.poisson(cusNumPer);

		//this is a dialog at each unit time during the open-time of the bank
		dialog<<"-----------------------------------------------------------------------"<<endl
			<<"                          Dialog at time: "<<I<<endl;

			//the queue system follows the tracks of each customer's movements
			//and prints them during the progress
			if(track)
				dialog << "[Customer Movements]\n"<<endl;
			
		//customers enqueue
		for(int j=0;j<=randCusNum;j++)
			queueSys.pressDevice(new Customer(I,rand()%cusCost+1));// each customer's service-time is random, too

		//then it's in bank's turn, queue system arranges the work
		//in each unit time(minute),queue system asks all windows to work and reply whether they are spare 
		//if were, arranges enqueued-customers to windows
		queueSys.work();
		
		//print real-time dynamic information of all windows
		dialog<<"\n[Window Information]\n"<<endl;
		queueSys.printWinDynInfo();

		dialog<<"-----------------------------------------------------------------------"<<endl;
		if(printDynamic)
		{
			sleep(1);
			system("clear");
		}
	}


	//this is a dialog after the bank shutdowns
	//why?
	//becase when the bank is time to shutdown, maybe there are still some customers being serviced
	//thus the bank need to finish the work before shutdown
	//at the same time the bank shutdowns the queue system
	//so enqueued-customers cannot get services and then leave the bank 
	dialog << "-----------------------------------------------------------------------"<<endl
			<< "                 Dialog at time: "<<bankCloseTime<<endl;
	dialog << "\nBank is going to shutdown and no one enqueue\n" << endl;
	if(track)
		dialog << "[Customer Movements]\n" << endl;
	queueSys.closeQueueSystem();
	queueSys.finishWork();

	dialog <<"\nAll work done!\nBank shutdowned!\n"<<endl;
	
	//calculate served-customers' total waiting-time and the average
	int totalServedWaitingTime = 0;
	int servedSize = servedCusVect.size();
	double averServedWaitingTime = 0;
	for(int i=0;i<servedSize;i++)
		totalServedWaitingTime += servedCusVect[i]->dequeueTime() - servedCusVect[i]->enqueueTime();
	if(servedSize != 0)	
		averServedWaitingTime = (double)totalServedWaitingTime/servedSize;

	//calculate unserved-customers' total waiting-time and the average
	int totalUnservedWaitingTime = 0;
	int unservedSize = unservedCusVect.size();
	double averUnservedWaitingTime = 0;
	for(int i=0;i<unservedSize;i++)
		totalUnservedWaitingTime += unservedCusVect[i]->dequeueTime() - unservedCusVect[i]->enqueueTime();
	if(unservedSize != 0)
		averUnservedWaitingTime = (double)totalUnservedWaitingTime/unservedSize;

	//calculate all customers' total waiting-time and the average
	int totalWaitingTime = totalServedWaitingTime + totalUnservedWaitingTime;
	int totalSize = Customer::enqueueNum();
	double averWaitingTime = 0;
	if(totalSize != 0)
		averWaitingTime = (double)totalWaitingTime/totalSize;

	// this is a final dialog
	dialog<<"======================================================================="<<endl
		<<"                            FINAL DIALOG\n"
		<<"\n[statistic information]\n"<<endl;
	
	dialog  << setw(60) << left << "The number of total enqueued-customers:" << totalSize << endl
			<< setw(60) << left << "The number of served-customers:" << servedSize << endl
			<< setw(60) << left << "The number of unserved-customers:" << unservedSize << endl
		 	<< setw(60) << left << "The total-waiting time of served-customers:" << totalServedWaitingTime << endl
			<< setw(60) << left << "The average waiting time of served-customers:" << averServedWaitingTime << endl
			<< setw(60) << left << "The total-waiting time of unserved-customers:" << totalUnservedWaitingTime << endl
			<< setw(60) << left << "The average waiting time of unserved-customers:" << averUnservedWaitingTime << endl
			<< setw(60) << left << "The total-waiting time of all customers:" << totalWaitingTime << endl
			<< setw(60) << left << "The average waiting time of all customers:" << averWaitingTime << "\n\n";
	queueSys.printWinCon();
	dialog<<"======================================================================="<<endl;

	dialog.close();
	cout << "DIALOG.txt has been created!\n";
	system("gedit DIALOG.txt &");
	return 0;
}
