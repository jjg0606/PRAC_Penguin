#include "pgGameMgr.h"

using namespace std;

pgGameMgr pgGameMgr::instance;

int pgGameMgr::getID()
{
	return cntUserID++;
}

void pgGameMgr::getRoomInfo(int roomNum, bool& isPlaying, int& cntPlayer)
{
	gameRoomArr[roomNum].getRoomInfo(isPlaying, cntPlayer);
}

bool pgGameMgr::getInRoom(int roomNum, pgGameUser* user)
{
	if (roomNum < 0 || roomNum >= MAX_LOBBY)
	{
		return false;
	}

	return gameRoomArr[roomNum].JoinPlayer(user);
}

void pgGameMgr::userOutRoom(int roomNum, int ID)
{
	if (roomNum <0 || roomNum >= MAX_LOBBY)
	{
		return;
	}

	gameRoomArr[roomNum].DisconnectPlayer(ID);
}

void pgGameMgr::RdyStateChanged(int roomNum)
{
	if (roomNum < 0 || roomNum >= MAX_LOBBY)
	{
		return;
	}

	gameRoomArr[roomNum].chkStartCondition();
}

void pgGameMgr::breakBlock(int roomNum, int userID, int row, int col)
{
	if (roomNum < 0 || roomNum >= MAX_LOBBY)
	{
		return;
	}

	gameRoomArr[roomNum].getPlayerInput(userID, row, col);
}

void pgGameMgr::BroadCastChat(int roomNum, int useidx, int length, wchar_t* buf)
{
	if (roomNum < 0 || roomNum >= MAX_LOBBY)
	{
		return;
	}
	gameRoomArr[roomNum].broadCastChat(useidx, length, buf);
}