#include "SocketMain.h"
#include "PACKET.h"
#include "pgGameUser.h"
#include <iostream>
#include <random>
#include <time.h>


void DisableNagle(SOCKET& client_sock);

void SocketMain(SOCKET& client_sock, SOCKADDR_IN& client_addr,bool& isStart)
{	
	srand(time(NULL));
	DisableNagle(client_sock);
	int addrlen;

	addrlen = sizeof(client_addr);
	getpeername(client_sock, (SOCKADDR*)&client_addr, &addrlen);

	pgGameUser* user = new pgGameUser(client_sock,isStart);
	Sleep(100);
	while (isStart)
	{	
		user->Update();		
	}
	delete user;
}

void DisableNagle(SOCKET& client_sock)
{
	int option = 1;
	setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));
}