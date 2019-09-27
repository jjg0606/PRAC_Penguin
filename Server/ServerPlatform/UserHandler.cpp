#include "UserHandler.h"
#include "SocketMain.h"
using namespace std;

UserHandler::UserHandler()
	: isStart(false), threadp(nullptr)
{
}

void UserHandler::Init(SOCKET client_sock, SOCKADDR_IN client_addr)
{
	this->client_sock = client_sock;
	this->client_addr = client_addr;

	threadp = new thread(
		[this]()
	{
		unique_lock<mutex> ul(this->startMutex);
		this->startEvent.wait(ul, [this]() {return this->isStart; });
		SocketMain(this->client_sock, this->client_addr, isStart);
		this->EndThread();
	});
}

void UserHandler::Start()
{
	isStart = true;
	this->startEvent.notify_one();
}

void UserHandler::EndThread()
{
	this->isStart = false;
	closesocket(client_sock);
	ThreadPool::instance.WorkingEnd(this);
}


void UserHandler::ForceEnd()
{
	this->isStart = false;
	threadp->join();
}


