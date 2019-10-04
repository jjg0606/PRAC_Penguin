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

pgGameRoom* pgGameMgr::getInRoom(int roomNum, pgGameUser* user)
{
	if (roomNum < 0 || roomNum >= MAX_LOBBY)
	{
		return false;
	}

	if (gameRoomArr[roomNum].JoinPlayer(user))
	{
		return (gameRoomArr + roomNum);
	}
	else
	{
		return nullptr;
	}
}

pgGameRoom* pgGameMgr::getInRoomAsWatcher(int roomNum, pgGameUser* user)
{
	if (roomNum < 0 || roomNum >= MAX_LOBBY)
	{
		return false;
	}

	if (gameRoomArr[roomNum].JoinWatcher(user))
	{
		return (gameRoomArr + roomNum);
	}
	else
	{
		return nullptr;
	}
}

void pgGameMgr::GetInLobby(pgGameUser* user)
{
	lbyroom.IntoLobby(user);
}

void pgGameMgr::GetOutLobby(pgGameUser* user)
{
	lbyroom.OutLobby(user);
}

void pgGameMgr::BroadCastToLobby(const std::wstring& name, int msgLength, wchar_t* buf)
{
	lbyroom.BroadCastChatMsg(name, msgLength, buf);
}