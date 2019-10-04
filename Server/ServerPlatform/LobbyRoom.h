#pragma once
#include <set>
#include <shared_mutex>

class pgGameUser;

class LobbyRoom
{
	std::shared_mutex userMutex;
	std::set<pgGameUser*> userSet;
public:
	void IntoLobby(pgGameUser* user);
	void OutLobby(pgGameUser* user);
	void BroadCastChatMsg(const std::wstring& id,int msgLength,wchar_t* buf);
};