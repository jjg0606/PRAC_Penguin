#pragma once
#include "pgGameRoom.h"

#include "PACKET.h"

class pgGameMgr
{
private:
	pgGameRoom gameRoomArr[MAX_LOBBY];
	int cntUserID = 0;

public:
	void getRoomInfo(int roomNum, bool& isPlaying, int& cntPlayer);
	bool getInRoom(int roomNum, pgGameUser* user);
	void userOutRoom(int roomNum, int ID);
	void RdyStateChanged(int roomNum);
	void breakBlock(int roomNum, int userID, int row, int col);
	void BroadCastChat(int roomNum, int useidx, int length, wchar_t* buf);
	static pgGameMgr instance;
	int getID();
};