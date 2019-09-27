#include "ThreadPool.h"

using namespace std;

void ThreadPool::Init(int maxTh)
{
	// prevent duplicated init
	if (!waitingThread.empty() || !workingThread.empty())
	{
		return;
	}
	
	for (int i = 0; i < maxTh; i++)
	{
		waitingThread.push_back(new UserHandler());
	}
	
}

bool ThreadPool::ConnectUser(SOCKET client, SOCKADDR_IN addr)
{
	if (waitingThread.empty())
	{
		return false;
	}

	UserHandler* handler = waitingThread.front();
	waitingThread.pop_front();
	handler->Init(client, addr);
	handler->Start();
	workingThread.insert(handler);

	return true;	
}

void ThreadPool::WorkingEnd(UserHandler* handler)
{
	auto iter = workingThread.find(handler);
	if (iter != workingThread.end())
	{
		workingThread.erase(iter);
	}

	waitingThread.push_back(handler);
}

ThreadPool::~ThreadPool()
{
	for (auto iter = waitingThread.begin(); iter != waitingThread.end(); iter++)
	{
		delete *iter;
	}
	waitingThread.clear();
	for (auto iter = workingThread.begin(); iter != workingThread.end(); iter++)
	{
		(*iter)->ForceEnd();
	}
	workingThread.clear();
}

ThreadPool ThreadPool::instance;