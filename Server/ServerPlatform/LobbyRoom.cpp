#include "LobbyRoom.h"
#include "pgGameUser.h"


void LobbyRoom::IntoLobby(pgGameUser* user)
{
	std::unique_lock<std::shared_mutex> ul(userMutex);
	userSet.insert(user);
}

void LobbyRoom::OutLobby(pgGameUser* user)
{
	std::unique_lock<std::shared_mutex> ul(userMutex);	
	userSet.erase(userSet.find(user));
}

void LobbyRoom::BroadCastChatMsg(const std::wstring& id, int msgLength, wchar_t* buf)
{
	std::shared_lock<std::shared_mutex> sl(userMutex);
	for (auto iter = userSet.begin(); iter != userSet.end(); iter++)
	{
		(*iter)->SendLobbyChatMsgTo(id, msgLength, buf);
	}
}