#include "pgGameUser.h"
#include "pgGameMgr.h"
#include <iostream>

pgGameUser::pgGameUser(SOCKET& sock,bool& isStart)
	: sock(sock), isStart(isStart)
{
	this->ID = pgGameMgr::instance.getID();
	int getRand = rand();
	//this->avatarNum = (getRand%4);
	buf = new char[BUFSIZE + 1];
	memset(buf, 0, sizeof(char)*(BUFSIZE + 1));
}

void pgGameUser::ReceivePacket(int size)
{
	bufIdx += size;
	if (bufIdx < sizeof(PACKET_HEADER))
	{
		return;
	}

	PACKET_HEADER pHeader;
	memcpy(&pHeader, buf, sizeof(pHeader));

	// ¸ðÀÚ¶ö¶§
	if (bufIdx < pHeader.size)
	{
		return;
	}

	switch (pHeader.type)
	{

	case PACKET_TYPE_SYSTEM:
	{
		PACKET_SYSTEM syspac;
		memcpy(&syspac, buf, pHeader.size);
		if (syspac.system_msg == SYSTEM_MSG_CLIENT_READY)
		{
			state = pgUserState::BEFORE_LOGIN;
		}
	}
	break;

	case PACKET_TYPE_LOGIN_INFO:
	{
		PACKET_LOGIN_INFO loginpac;
		memcpy(&loginpac, buf, pHeader.size);
		avatarNum = loginpac.avatar;
		name = loginpac.name;
		state = pgUserState::INIT;
	}
	break;
	
	case PACKET_TYPE_LOBBY_IN:
	{
		isReady = false;
		PACKET_LOBBY_IN lobpac;
		memcpy(&lobpac, buf, pHeader.size);
		pgGameMgr::instance.userOutRoom(gameRoomNum, ID);
		state = pgUserState::IN_LOBBY;
		Sleep(10);
		if (pgGameMgr::instance.getInRoom(lobpac.index, this))
		{
			gameRoomNum = lobpac.index;
			state = pgUserState::READY_FOR_GAME;
		}
		else
		{
			SendLobbyInfo();
			lastSend = std::chrono::system_clock::now();
		}
	}
	break;

	case PACKET_TYPE_GAME_READY:
		RdyStateChange();
	break;

	case PACKET_TYPE_BREAK_BLOCK:
	{
		PACKET_BREAK_BLOCK bbpac;
		memcpy(&bbpac, buf, pHeader.size);
		pgGameMgr::instance.breakBlock(gameRoomNum, ID, bbpac.idx[0] / COLS, bbpac.idx[0] % COLS);
	}
	break;

	case PACKET_TYPE_CHAT_MSG:
	{
		PACKET_CHAT_MSG chatpac;
		memcpy(&chatpac, buf, pHeader.size);
		pgGameMgr::instance.BroadCastChat(gameRoomNum, ID, chatpac.chatLength, chatpac.msg);
		//TODO
	}
	break;

	default:
		cutBuf(bufIdx);
		return;
	}
	cutBuf(pHeader.size);
}

void pgGameUser::cutBuf(int size)
{
	int back = size;
	int front = 0;
	while (back < bufIdx)
	{
		buf[front] = buf[back];
		buf[back] = 0;
		back++;
		front++;
	}
	bufIdx -= size;	
}

void pgGameUser::SendToUser(void* buf, int size)
{
	send(sock, (const char*)buf, size, 0);
}

void pgGameUser::Update()
{
	if (!RecvFunc())
	{
		Release();
		return;
	}

	std::chrono::duration<float> deltaTime = std::chrono::system_clock::now() - lastSend;

	if (state == pgUserState::BEFORE_READY_SIGNAL)
	{
		if (deltaTime.count() > 5.0f)
		{
			SendReadySignal();
			lastSend = std::chrono::system_clock::now();
		}
	}
	else if (state == pgUserState::INIT)
	{
		SendLobbyInfo();
		lastSend = std::chrono::system_clock::now();
		pgUserState::IN_LOBBY;
	}
}

bool pgGameUser::RecvFunc()
{
	int length;
	length = recv(sock, (buf+bufIdx), BUFSIZE - bufIdx, 0);
	if (length == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return true;
		}
		else
		{
			return false;
		}		
	}
	else if (length == 0)
	{
		return false;
	}
	
	ReceivePacket(length);
	return true;
}

void pgGameUser::Release()
{
	isStart = false;
	delete buf;
	pgGameMgr::instance.userOutRoom(gameRoomNum, ID);
}

void pgGameUser::SendReadySignal()
{
	PACKET_SYSTEM syspac;
	syspac.header.type = PACKET_TYPE_SYSTEM;
	syspac.header.size = sizeof(syspac);
	syspac.system_msg = SYSTEM_MSG_SERVER_READY;
	SendToUser(&syspac, sizeof(syspac));
}

void pgGameUser::SendLobbyInfo()
{
	PACKET_LOBBY_INFO lobbypac;
	lobbypac.header.type = PACKET_TYPE_LOBBY_INFO;
	lobbypac.header.size = sizeof(lobbypac);
	lobbypac.roomNum = MAX_LOBBY;
	for (int i = 0; i < MAX_LOBBY; i++)
	{
		bool isPlaying;
		int playerNum;
		pgGameMgr::instance.getRoomInfo(i, isPlaying, playerNum);

		lobbypac.isPlaying[i] = isPlaying;
		lobbypac.playerNum[i] = playerNum;			
	}
	SendToUser(&lobbypac, sizeof(lobbypac));
}


void pgGameUser::SendLobbyPlayers(int playerNum, pgGameUser* list[4])
{
	PACKET_LOBBY_PLAYERS lppac;
	lppac.header.type = PACKET_TYPE_LOBBY_PLAYERS;
	lppac.header.size = sizeof(lppac);
	lppac.playerNum = playerNum;
	for (int i = 0; i < playerNum; i++)
	{
		lppac.avatar[i] = list[i]->avatarNum;
		lppac.isReady[i] = list[i]->isReady;
		if (list[i]->ID == this->ID)
		{
			lppac.myNum = i;
		}
	}
	SendToUser(&lppac, sizeof(lppac));
}

void pgGameUser::RdyStateChange()
{
	this->isReady = !isReady;
	pgGameMgr::instance.RdyStateChanged(gameRoomNum);
}

void pgGameUser::SendBlockMap(std::vector<std::vector<int>>& map,int turnIdx,int command)
{
	PACKET_BLOCK_SETTING blockpac;
	blockpac.header.type = PACKET_TYPE_BLOCK_SETTING;
	blockpac.header.size = sizeof(blockpac);
	blockpac.trunIdx = turnIdx;
	blockpac.command = command;
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			blockpac.blockState[r * COLS + c] = map[r][c];
		}
	}
	state = pgUserState::IN_GAME;
	SendToUser(&blockpac, sizeof(blockpac));
}

void pgGameUser::SendBreakInfo(int breakNum, int* breakArr)
{
	PACKET_BREAK_BLOCK brpac;
	brpac.header.type = PACKET_TYPE_BREAK_BLOCK;
	brpac.nums = breakNum;
	for (int i = 0; i < breakNum; i++)
	{
		brpac.idx[i] = breakArr[i];
	}
	brpac.header.size = sizeof(brpac.header) + sizeof(brpac.nums) + sizeof(int) * breakNum;
	SendToUser(&brpac, brpac.header.size);
}

void pgGameUser::SendTurnInfo(int turnp, int command)
{
	PACKET_TURN_MSG turnmsg;
	turnmsg.header.type = PACKET_TYPE_TURN_MSG;
	turnmsg.header.size = sizeof(turnmsg);
	turnmsg.turnp = turnp;
	turnmsg.command = command;

	SendToUser(&turnmsg, sizeof(turnmsg));
}

void pgGameUser::OnGameEnd()
{
	this->isReady = false;
	this->state = pgUserState::READY_FOR_GAME;
}

void pgGameUser::SendChatMsgTo(int idx, int length, wchar_t* buf)
{
	PACKET_CHAT_MSG chatpac;
	chatpac.header.type = PACKET_TYPE_CHAT_MSG;
	chatpac.header.size = sizeof(PACKET_HEADER) + sizeof(int) * 2 + sizeof(wchar_t) * length;
	chatpac.chatLength = length;
	chatpac.playerIdx = idx;
	for (int i = 0; i < length; i++)
	{
		chatpac.msg[i] = buf[i];
	}

	SendToUser(&chatpac, chatpac.header.size);
}