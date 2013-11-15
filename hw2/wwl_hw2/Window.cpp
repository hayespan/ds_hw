#include "Window.h"
#include "Client.h"
#include <queue>
using namespace std;
int Window::ids_ = 1;
bool Window::multiQueueMode_ = 0;
queue<Client*> Window::headQueue_;
Window::Window()
{
	id_ = ids_++;
	serCliNum_ = 0;
	serCliTime_ = 0;
	serCli_ = 0;
	spare_ = 1;
}
int Window::getId()
{
	return id_;
}
int Window::getSerCliNum()
{
	return serCliNum_;
}
int Window::getSerCliTime()
{
	return serCliTime_;
}
Client* Window::getSerCli()
{
	return serCli_;
}
int Window::getDivQueueSize()
{
	return divisionQueue_.size();
}
bool Window::getMode()
{
	return multiQueueMode_;
}
bool Window::getSpare()
{
	return spare_;
}
void Window::setMode(bool t)
{
	multiQueueMode_ = t;
}
void Window::setSpare(bool spare)
{
	spare_ = spare;
}
void Window::setSerCli(Client *t)
{
	delete serCli_;
	serCli_ = t;
	serCliNum_ ++;
	serCliTime_ += t->getSerTime();
}
void Window::enDivQueue(Client *t)
{
	divisionQueue_.push(t);
}
Client* Window::deDivQueue()
{
	Client * t = divisionQueue_.front();
	divisionQueue_.pop();	
	return t;
}
int Window::getHeadQueueSize()
{
	return headQueue_.size();
}
void Window::enHeadQueue(Client *t)
{
	headQueue_.push(t);
}
Client* Window::deHeadQueue()
{
	Client * t = headQueue_.front();
	headQueue_.pop();
	return t;
}