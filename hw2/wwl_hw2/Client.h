#ifndef CLIENT_H
#define CLIENT_H
// 客户类
class Client
{
public:
	Client(int curTime, int serTime);
	int getEnqueueTime();
	int getDequeueTime();
	int getSerTime();
	int getIndex();
	int getWaitingTime();


	void setDequeueTime(int dequeueTime);
	static int getIndexes();
private:
	int enqueueTime_;
	int dequeueTime_;
	int serTime_;
	int index_;

	static int indexes_; 
};
#endif