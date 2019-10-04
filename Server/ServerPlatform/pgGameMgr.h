#pragma once
#include "pgGameRoom.h"
#include "PACKET.h"
#include "LobbyRoom.h"

class pgGameMgr
{
private:
	pgGameRoom gameRoomArr[MAX_LOBBY];
	LobbyRoom lbyroom;
	int cntUserID = 0;

public:
	void getRoomInfo(int roomNum, bool& isPlaying, int& cntPlayer);
	pgGameRoom* getInRoom(int roomNum, pgGameUser* user);
	pgGameRoom* getInRoomAsWatcher(int roomNum, pgGameUser* user);
	void GetInLobby(pgGameUser* user);
	void GetOutLobby(pgGameUser* user);
	void BroadCastToLobby(const std::wstring& name,int msgLength,wchar_t* buf);
	static pgGameMgr instance;
	int getID();
};
