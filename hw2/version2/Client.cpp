#include "Client.h"
using namespace std;
int Client::indexes_ = 0;
Client::Client(int curTime, int serTime)
{
	enqueueTime_ = curTime;
	dequeueTime_ = 0;
	serTime_ = serTime;
	index_ = ++indexes_;
}
int Client::getEnqueueTime()
{
	return enqueueTime_;
}
int Client::getDequeueTime()
{
	return dequeueTime_;
}
int Client::getSerTime()
{
	return serTime_;
}
int Client::getIndex()
{
	return index_;
}
int Client::getWaitingTime()
{
	return dequeueTime_ - enqueueTime_;
}
void Client::setDequeueTime(int dequeueTime)
{
	dequeueTime_ = dequeueTime;
}
int Client::getIndexes()
{
	return indexes_;
}