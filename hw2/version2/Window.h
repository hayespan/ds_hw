#ifndef WINDOW_H
#define WINDOW_H
#include "Client.h"
#include <queue>
using namespace std;
// 窗口类
class Window
{
public:
	Window();
	int getId();
	int getSerCliNum();
	int getSerCliTime();
	Client* getSerCli();
	int getDivQueueSize();
	bool getSpare();

	void setSpare(bool spare);
	void setSerCli(Client *t);
	void enDivQueue(Client	*t);
	Client* deDivQueue();

	static int getHeadQueueSize();
	static bool getMode();

	static void setMode(bool t);
	static void enHeadQueue(Client *t);
	static Client* deHeadQueue();

private:
	int id_;
	int serCliNum_;
	int serCliTime_;
	Client *serCli_;
	queue<Client*> divisionQueue_;
	bool spare_;

	static int ids_;
	static bool multiQueueMode_;
	static queue<Client*> headQueue_;
};
#endif