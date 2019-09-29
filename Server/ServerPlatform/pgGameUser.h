#pragma once
#include <WinSock2.h>
#include "PACKET.h"
#include "EnumPgUserState.h"
#include <chrono>
#include <vector>
#include <list>
#include <string>
#define BUFSIZE 1024

class pgGameUser
{
private:
	
	int gameRoomNum = -1;
	SOCKET& sock;
	bool& isStart;
	std::wstring name;
	pgUserState state = pgUserState::BEFORE_READY_SIGNAL;
	std::chrono::time_point<std::chrono::system_clock> lastSend;
	void cutBuf(int size);
	bool RecvFunc();
	// signal
	void SendReadySignal();
	void SendLobbyInfo();
	void RdyStateChange();
	//void SendLobbyResult(int index);
	int bufIdx=0;
	char* buf;
public:
	bool isReady = false;
	int ID;
	int avatarNum;
	void Release();
	void ReceivePacket(int size);
	void SendToUser(void* buf, int size);
	void Update();
	pgGameUser(SOCKET& sock,bool& isStart);

	void OnGameEnd();
	void SendLobbyPlayers(int playerNum, pgGameUser* list[4]);
	void SendBlockMap(std::vector<std::vector<int>>& map,int turnIdx,int command);
	void SendBreakInfo(int breakNum, int* breakArr);
	void SendTurnInfo(int turnp, int command);
	void SendChatMsgTo(int idx, int length, wchar_t* buf);
};