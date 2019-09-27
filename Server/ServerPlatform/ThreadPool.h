#pragma once
#include <WinSock2.h>
#include "UserHandler.h"
#include <list>
#include <set>

class UserHandler;

class ThreadPool
{
private:
	std::list<UserHandler*> waitingThread;
	std::set<UserHandler*> workingThread;
public:
	static ThreadPool instance;
	void Init(int maxTh);
	bool ConnectUser(SOCKET client, SOCKADDR_IN addr);
	void WorkingEnd(UserHandler* handler);
	~ThreadPool();
};