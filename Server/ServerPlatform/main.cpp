#pragma comment(lib,"ws2_32")
#include "Logger.h"
#include "SettingLoader.h"
#include "ThreadPool.h"
#include <winsock2.h>
#include <iostream>
#include <random>
#include <time.h>

int main()
{
	Logger::Log("init");
	if (!SettingLoader::isLoaded())
	{
		Logger::Log("setting load failed");
		return 1;
	}
	

	int inet,protocol, port;
	inet = SettingLoader::getIPType();
	protocol = SettingLoader::getProtocol();
	port = SettingLoader::getPort();

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		Logger::Log("WSA startup failed");
		return 1;
	}

	SOCKET listen_sock = socket(inet, protocol, 0);
	if (listen_sock == INVALID_SOCKET)
	{
		Logger::Log("listen socket is invalid");
		return 1;
	}

	u_long on = 1;
	if (ioctlsocket(listen_sock, FIONBIO, &on) == SOCKET_ERROR)
	{
		Logger::Log("ioctl failed");
		return -1;
	}

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = inet;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);
	if (bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
	{
		Logger::Log("socket bind error");
		return 1;
	}

	if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR)
	{
		Logger::Log("socket listen failed");
		return 1;
	}

	ThreadPool::instance.Init(SettingLoader::getMaxThread());
	printf("server boot success port = %d \n", port);

	SOCKET client_sock;
	SOCKADDR_IN client_addr;
	int addrlen;

	bool isWorking = true;

	std::thread endCheck([&isWorking]() {
		int end = 0;
		while (isWorking)
		{
			std::cin >> end;
			if (end == -1)
			{
				isWorking = false;
			}
		}
	});

	

	while (isWorking)
	{
		addrlen = sizeof(client_addr);
		client_sock = accept(listen_sock, (SOCKADDR*)&client_addr, &addrlen);
		if(client_sock == INVALID_SOCKET)
		{ 
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}
			else
			{
				break;
			}
		}

		printf("클라이언트 접속 : IP주소 %s, 포트번호 %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		if (!ThreadPool::instance.ConnectUser(client_sock, client_addr))
		{
			printf("rejected\n");
		}
	}
	endCheck.join();
	closesocket(listen_sock);
	WSACleanup();
	Logger::Log("end server");
	return 0;
}