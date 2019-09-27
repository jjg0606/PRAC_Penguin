#pragma once
#include <WinSock2.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "ThreadPool.h"

class ThreadPool;

class UserHandler
{
protected:
	SOCKET client_sock;
	SOCKADDR_IN client_addr;
	std::mutex startMutex;
	bool isStart;
	std::thread* threadp;
	std::condition_variable startEvent;
	void EndThread();
public:
	void Init(SOCKET client_sock, SOCKADDR_IN client_addr);
	void Start();
	void ForceEnd();
	UserHandler();
};