#include "pgGameScene.h"

#include "EnumImg.h"
#include "EnumMouse.h"
#include "ProgramCore.h"


using namespace std;

extern SOCKET g_sock;

void pgGameScene::Update()
{
	ReadPacket(0);
	if(GetAsyncKeyState(VK_RETURN)&0x8000)
	{
		if (state == pgGameState::IN_GAME || state == pgGameState::READY_FOR_GAME)
		{
			SendChatMsg();
		}
	}
	Invalidate();
}

void pgGameScene::DrawScreen(HDC hdc)
{
	switch (state)
	{
	case pgGameState::IN_LOBBY:
		DrawLobby(hdc);
		break;
	case pgGameState::READY_FOR_GAME:
	case pgGameState::IN_GAME:
		DrawInGame(hdc);
		break;
	default:
		render.SetImg(imgMap[IMG_BACKGROUND]);
		render.SetPosition(0, 0);
		render.Render(hdc);
		break;
	}

}

void pgGameScene::DrawLobby(HDC hdc)
{
	int startPosX = 50;
	int startPosY = 50;
	FillRect(hdc, &getWinRect(), (HBRUSH)GetStockObject(WHITE_BRUSH));
	int width = 100;
	int height = 120;
	int rows = 2;
	int cols = 5;
	int dx = 20;
	int dy = 20;

	wchar_t charbuf[20];

	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int curX = startPosX + dx * c + width * c;
			int curY = startPosY + dy * r + height * r;
			MoveToEx(hdc,curX ,curY , NULL);
			LineTo(hdc, curX + width, curY);
			LineTo(hdc, curX + width, curY + height);
			LineTo(hdc, curX, curY + height);
			LineTo(hdc, curX, curY);
			
			if (lobbyVec[cols*r + c].first)
			{
				wsprintf(charbuf, TEXT("PLAYING NOW"));
			}
			else 
			{
				wsprintf(charbuf, TEXT("WAITING - %d"),lobbyVec[cols*r+c].second);
			}

			TextOut(hdc, curX, curY, charbuf, lstrlen(charbuf));
		}
	}

	render.SetImg(imgMap[IMG_BTN_REFRESH]);
	render.SetPosition(refreshBtn->xpos, refreshBtn->ypos);
	render.Render(hdc);
}

void pgGameScene::DrawInGame(HDC hdc)
{
	int startPosX = 50;
	int startPosY = 50;
	int imgWidth = imgMap[IMG_PENGUIN]->bmWidth;
	int imgHeight = imgMap[IMG_PENGUIN]->bmHeight;

	FillRect(hdc, &getWinRect(), (HBRUSH)GetStockObject(WHITE_BRUSH));
	render.SetImg(imgMap[IMG_STORM]);
	render.SetPosition(startPosX, startPosY);
	render.Render(hdc);

	if (state == pgGameState::IN_GAME)
	{
		for (int r = 0; r < ROWS; r++)
		{
			for (int c = 0; c < COLS; c++)
			{
				render.SetPosition(startPosX + imgWidth * c, startPosY + imgHeight * r);
				switch (blockMap[r][c])
				{
				case NULL_BLOCK:
					break;
				case WHITE_BLOCK:
					render.SetImg(imgMap[IMG_ICE_WHITE]);
					render.Render(hdc);
					break;
				case BLUE_BLOCK:
					render.SetImg(imgMap[IMG_ICE_BLUE]);
					render.Render(hdc);
					break;
				case PENGUIN_BLOCK:
					render.SetImg(imgMap[IMG_PENGUIN]);
					render.Render(hdc);
					break;
				}
			}
		}
	}
	DrawLobbyAvatar(hdc);
}

void pgGameScene::DrawLobbyAvatar(HDC hdc)
{
	int startPosX = 50 + imgMap[IMG_PENGUIN]->bmWidth * COLS + 50;
	int startPosY = 50;

	int width = imgMap[IMG_AVATAR1]->bmWidth;
	int height = imgMap[IMG_AVATAR1]->bmHeight;
	int dx = 10;
	int dy = 10;

	int index = 0;
	for (int r = 0; r < 2; r++)
	{
		for (int c = 0; c < 2; c++,index++)
		{
			if (index >= inGamePlayerAvatar.size())
			{
				continue;
			}

			render.SetImg(imgMap[IMG_AVATAR1 + inGamePlayerAvatar[index]]);
			render.SetPosition(startPosX + dx * c + width * c, startPosY + dy * r + height * r);
			render.Render(hdc);

			if (index == myIndex)
			{
				render.SetImg(imgMap[IMG_AURA_MY]);
				render.Render(hdc);
			}

			if (index == turnIdx && state == pgGameState::IN_GAME)
			{
				render.SetImg(imgMap[IMG_AURA_TURN]);
				render.Render(hdc);
			}
		}
	}

	if (isGameEnd)
	{
		Image* endImg = myIndex == turnIdx ? imgMap[IMG_WIN] : imgMap[IMG_LOSE];
		RECT& winrect = getWinRect();
		render.SetImg(endImg);
		render.SetPosition((winrect.right - endImg->bmWidth) / 2, (winrect.bottom - endImg->bmHeight) / 2);
		render.Render(hdc);
	}

	{
		int startX = 1000;
		int startY = 50;
		int dy = 30;
		for (int i = 0; i < chatMsgVec.size(); i++)
		{
			TextOut(hdc, startX, startPosY + dy * i, chatMsgVec[i].c_str(), chatMsgVec[i].length());
		}
	}

	if (state == pgGameState::IN_GAME)
	{
		switch (curCommand)
		{
		case COMMAND_BREAK_WHITE:
			render.SetImg(imgMap[IMG_CMD_WHITE]);
			break;
		case COMMAND_BREAK_BLUE:
			render.SetImg(imgMap[IMG_CMD_BLUE]);
			break;
		case COMMAND_BREAK_BOTH:
			render.SetImg(imgMap[IMG_CMD_BOTH]);
			break;
		case COMMAND_BREAK_NOTHING:
			render.SetImg(imgMap[IMG_CMD_NOTHING]);
			break;
		}
		render.SetPosition(50, 500);
		render.Render(hdc);
		return;
	}

	index = 0;
	for (int r = 0; r < 2; r++)
	{
		for (int c = 0; c < 2; c++, index++)
		{
			if (index >= inGamePlayerAvatar.size() || inGameReadyState[index] == false)
			{
				continue;
			}
			render.SetImg(imgMap[IMG_ON_READY]);
			render.SetPosition(startPosX + dx * c + width * c, startPosY + dy * r + height * r);
			render.Render(hdc);
		}
	}
	
	render.SetImg(imgMap[IMG_BTN_READY]);
	render.SetPosition(readyBtn->xpos,readyBtn->ypos);
	render.Render(hdc);

	render.SetImg(imgMap[IMG_BTN_EXIT]);
	render.SetPosition(exitBtn->xpos,exitBtn->ypos);
	render.Render(hdc);
}

void pgGameScene::OnMouseClick(int x, int y, int BTN)
{
	if (BTN != MOUSE_LEFT_BTN)
	{
		return;
	}

	switch (state)
	{
	case pgGameState::IN_LOBBY:
		LobbyMouseHandle(x, y);
		break;
	case pgGameState::READY_FOR_GAME:
	case pgGameState::IN_GAME:
		InGameMouseHandle(x, y);
		break;
	}
}

void pgGameScene::InGameMouseHandle(int x,int y)
{
	if (isGameEnd)
	{
		return;
	}

	int startPosX = 50;
	int startPosY = 50;
	int imgWidth = imgMap[IMG_PENGUIN]->bmWidth;
	int imgHeight = imgMap[IMG_PENGUIN]->bmHeight;

	if (state == pgGameState::IN_GAME)
	{
		if (x >= startPosX && x <= startPosX + imgWidth * COLS
			&& y >= startPosY && y <= startPosY + imgHeight * ROWS)
		{
			x -= startPosX;
			y -= startPosY;
			int row = y / imgHeight;
			int col = x / imgWidth;
			//
			SendBreakBlockMsg(row, col);
			//
			return;
		}
	}
	else if (state == pgGameState::READY_FOR_GAME)
	{
		if (readyBtn->isIn(x, y))
		{
			SendReadySignal();
		}
		else if (exitBtn->isIn(x, y))
		{
			SendLobbbyInMsg(-1);
			state = pgGameState::IN_LOBBY;
			chatMsgVec.clear();
			DestroyWindow(hChat);
			
		}
	}	
}

void pgGameScene::LobbyMouseHandle(int x, int y)
{
	int startPosX = 50;
	int startPosY = 50;
	int width = 100;
	int height = 120;
	int rows = 2;
	int cols = 5;
	int dx = 20;
	int dy = 20;

	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int curX = startPosX + dx * c + width * c;
			int curY = startPosY + dy * r + height * r;
			int lastX = curX + width;
			int lastY = curY + height;
			if (x >= curX && x <= lastX && y >= curY && y <= lastY && !lobbyVec[cols*r+c].first)
			{
				// TODO
				SendLobbbyInMsg(cols*r + c);
				return;
			}
		}
	}

	if (refreshBtn->isIn(x, y))
	{
		SendLobbbyInMsg(-1);
	}
}

void pgGameScene::OnSocketEvent(UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	int addrlen = 0;
	int retval = 0;
	
	if (WSAGETSELECTERROR(lParam))
	{
		int err_code = WSAGETSELECTERROR(lParam);
		return;
	}
	

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
	{
		retval = recv(wParam, readBuf+bufIdx, BUFSIZE - bufIdx, 0);
		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				return;
			}
		}
		else if (retval == 0)
		{
			return;
		}
		bufIdx += retval;
		ReadPacket(0);
		//ReadPacket(retval);
	}
	break;
	}
}

void pgGameScene::Init()
{
	int option = 1;
	setsockopt(g_sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

	LoadImgs();
	blockMap.resize(ROWS);
	for (int i = 0; i < ROWS; i++)
	{
		blockMap[i].resize(COLS, 0);
	}

	//SendInitMsg();
	state = pgGameState::INIT;
	RECT& winRect = getWinRect();
	mainCam = new Camera(0, 0, winRect.right, winRect.bottom);
	ProgramCore::instance.SetUpdateIntersec(0);
	render.Init(nullptr, RGB(255, 0, 255), 0);

	lobbyVec.resize(10);

	int startPosX = 50 + imgMap[IMG_PENGUIN]->bmWidth * COLS + 50;
	readyBtn = new pgBtn(startPosX, 550, imgMap[IMG_BTN_READY]->bmWidth, imgMap[IMG_BTN_READY]->bmHeight);
	exitBtn = new pgBtn(startPosX + readyBtn->width + 10, 550, imgMap[IMG_BTN_EXIT]->bmWidth, imgMap[IMG_BTN_EXIT]->bmHeight);
	refreshBtn = new pgBtn(670,310 - imgMap[IMG_BTN_REFRESH]->bmHeight,imgMap[IMG_BTN_REFRESH]->bmWidth,imgMap[IMG_BTN_REFRESH]->bmHeight);

	memset(readBuf, 0, sizeof(readBuf));
}

void pgGameScene::SendBreakBlockMsg(int row, int col)
{
	if (turnIdx != myIndex)
	{
		return;
	}

	if (curCommand == COMMAND_NOTHING)
	{
		return;
	}

	if (row < 0 || row >= ROWS || col < 0 || col >= COLS || blockMap[row][col] == NULL_BLOCK)
	{
		return;
	}
	
	if (curCommand == COMMAND_BREAK_WHITE && blockMap[row][col] == BLUE_BLOCK)
	{
		return;
	}

	if (curCommand == COMMAND_BREAK_BLUE && blockMap[row][col] == WHITE_BLOCK)
	{
		return;
	}

	PACKET_BREAK_BLOCK brpac;
	brpac.header.type = PACKET_TYPE_BREAK_BLOCK;
	brpac.header.size = sizeof(brpac.header) + sizeof(brpac.idx) + sizeof(int);
	brpac.nums = 1;
	brpac.idx[0] = row * COLS + col;
	
	send(g_sock, (const char*)&brpac, sizeof(brpac), 0);
}

void pgGameScene::SendInitMsg()
{
	// send init message to server
	PACKET_SYSTEM syspac;
	syspac.header.type = PACKET_TYPE_SYSTEM;
	syspac.header.size = sizeof(syspac);
	syspac.system_msg = SYSTEM_MSG_CLIENT_READY;

	send(g_sock, (const char*)&syspac, sizeof(syspac), 0);
}

void pgGameScene::SendLobbbyInMsg(int index)
{
	PACKET_LOBBY_IN lobbypac;
	lobbypac.header.type = PACKET_TYPE_LOBBY_IN;
	lobbypac.header.size = sizeof(lobbypac);
	lobbypac.index = index;

	send(g_sock, (const char*)&lobbypac, sizeof(lobbypac), 0);
}

void pgGameScene::SendReadySignal()
{
	PACKET_HEADER header;
	header.type = PACKET_TYPE_GAME_READY;
	header.size = sizeof(header);

	send(g_sock, (const char*)&header, sizeof(header), 0);
}

void pgGameScene::LoadImgs()
{
	imgMap[IMG_ICE_BLUE] = new Image(L"resource/ice_blue.bmp");
	imgMap[IMG_ICE_WHITE] = new Image(L"resource/ice_white.bmp");
	imgMap[IMG_PENGUIN] = new Image(L"resource/penguin.bmp");
	imgMap[IMG_BACKGROUND] = new Image(L"resource/back.bmp");
	imgMap[IMG_STORM] = new Image(L"resource/storm.bmp");
	imgMap[IMG_AVATAR1] = new Image(L"resource/avatar.bmp");
	imgMap[IMG_AVATAR2] = new Image(L"resource/avatar_egg.bmp");
	imgMap[IMG_AVATAR3] = new Image(L"resource/avatar_egg2.bmp");
	imgMap[IMG_AVATAR4] = new Image(L"resource/avatar_tomato.bmp");
	imgMap[IMG_BTN_READY] = new Image(L"resource/ready_btn.bmp");
	imgMap[IMG_BTN_EXIT] = new Image(L"resource/exit_btn.bmp");
	imgMap[IMG_ON_READY] = new Image(L"resource/avatar_on_ready.bmp");	
	imgMap[IMG_AURA_MY] = new Image(L"resource/aura_my.bmp");
	imgMap[IMG_AURA_TURN] = new Image(L"resource/aura_turn.bmp");
	imgMap[IMG_CMD_WHITE] = new Image(L"resource/command_white.bmp");
	imgMap[IMG_CMD_BLUE] = new Image(L"resource/command_blue.bmp");
	imgMap[IMG_CMD_BOTH] = new Image(L"resource/command_both.bmp");
	imgMap[IMG_CMD_NOTHING] = new Image(L"resource/command_nothing.bmp");
	imgMap[IMG_WIN] = new Image(L"resource/win.bmp");
	imgMap[IMG_LOSE] = new Image(L"resource/lose.bmp");
	imgMap[IMG_BTN_REFRESH] = new Image(L"resource/refresh_btn.bmp");
}

void pgGameScene::Release()
{
	for (auto iter = imgMap.begin(); iter != imgMap.end(); iter++)
	{
		delete (*iter).second;
	}
	delete readyBtn;
	delete exitBtn;
}

void pgGameScene::ReadPacket(int length)
{
	bufIdx += length;
	if (bufIdx < sizeof(PACKET_HEADER))
	{
		return;
	}

	PACKET_HEADER header;
	memcpy(&header, readBuf, sizeof(header));

	if (bufIdx < header.size)
	{
		return;
	}

	if (header.size < sizeof(header)||BUFSIZE < length)
	{
		return;
	}

	switch (header.type)
	{
	case PACKET_TYPE_SYSTEM:
	{
		PACKET_SYSTEM syspac;
		memcpy(&syspac, readBuf, sizeof(syspac));
		if (syspac.system_msg == SYSTEM_MSG_SERVER_READY)
		{
			SendInitMsg();
		}
	}
	break;

	case PACKET_TYPE_BLOCK_SETTING:
	{
		state = pgGameState::IN_GAME;
		PACKET_BLOCK_SETTING blockset;
		memcpy(&blockset, readBuf, header.size);
		for (int r = 0; r < ROWS; r++)
		{
			for (int c = 0; c < COLS; c++)
			{
				blockMap[r][c] = blockset.blockState[COLS * r + c];
			}
		}
		turnIdx = blockset.trunIdx;
		curCommand = blockset.command;
	}
	break;

	case PACKET_TYPE_LOBBY_INFO:
	{
		PACKET_LOBBY_INFO lobbypac;
		memcpy(&lobbypac, readBuf, header.size);
		for (int i = 0; i < lobbypac.roomNum; i++)
		{
			lobbyVec[i].first = lobbypac.isPlaying[i];
			lobbyVec[i].second = lobbypac.playerNum[i];
		}
		state = pgGameState::IN_LOBBY;
	}
	break;

	//case PACKET_TYPE_LOBBY_IN:
	//{
	//	PACKET_LOBBY_IN lobpac;
	//	memcpy(&lobpac, readBuf, header.size);
	//	if (lobpac.index > 0)
	//	{
	//		state = pgGameState::READY_FOR_GAME;
	//	}
	//}
	//break;

	case PACKET_TYPE_LOBBY_PLAYERS:
	{
		PACKET_LOBBY_PLAYERS lppac;
		memcpy(&lppac, readBuf, header.size);
		inGamePlayerAvatar.resize(lppac.playerNum);
		inGameReadyState.resize(lppac.playerNum);
		for (int i = 0; i < lppac.playerNum; i++)
		{
			inGamePlayerAvatar[i] = lppac.avatar[i];
			inGameReadyState[i] = lppac.isReady[i];
		}
		this->myIndex = lppac.myNum;
		isGameEnd = false;		
		if (state == pgGameState::IN_LOBBY)
		{
			CreateChat();
			state = pgGameState::READY_FOR_GAME;
		}
	}
	break;

	case PACKET_TYPE_TURN_MSG:
	{
		PACKET_TURN_MSG tupac;
		memcpy(&tupac, readBuf, header.size);
		turnIdx = tupac.turnp;
		curCommand = tupac.command;
	}
	break;

	case PACKET_TYPE_BREAK_BLOCK:
	{
		PACKET_BREAK_BLOCK brpac;
		memcpy(&brpac, readBuf, header.size);
		for (int i = 0; i < brpac.nums; i++)
		{
			int row = brpac.idx[i] / COLS;
			int col = brpac.idx[i] % COLS;
			blockMap[row][col] = NULL_BLOCK;
		}
		if (blockMap[ROWS / 2][COLS / 2] == NULL_BLOCK)
		{
			// game ��
			isGameEnd = true;
		}
	}
	break;

	case PACKET_TYPE_CHAT_MSG:
	{
		PACKET_CHAT_MSG chatpac;
		memcpy(&chatpac, readBuf, header.size);
		std::wstring cur;
		cur.append(L"Player");
		cur.append(to_wstring(chatpac.playerIdx + 1));
		cur.append(L" : ");
		for (int i = 0; i < chatpac.chatLength; i++)
		{
			cur.push_back(chatpac.msg[i]);
		}
		chatMsgVec.emplace_back(cur);
	}
	break;

	default:
		CutBuf(bufIdx);
		return;
	}

	CutBuf(header.size);
}

void pgGameScene::CutBuf(int length)
{
	int back = length;
	int front = 0;
	while (back < bufIdx && back <BUFSIZE)
	{
		readBuf[front] = readBuf[back];
		readBuf[back] = 0;
		back++;
		front++;
	}
	bufIdx -= length;
}

void pgGameScene::ClearReadBuf()
{
	memset(readBuf, 0, BUFSIZE);
}

void pgGameScene::OnMenuMessage(WPARAM wParam)
{
}

void pgGameScene::CreateChat()
{
	hChat = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL, 1000, 550, 300, 30,ProgramCore::instance.getHWND(), (HMENU)idChat, ProgramCore::instance.getGinst(),NULL);
	SendMessage(hChat, EM_LIMITTEXT, (WPARAM)MAX_CHAT_LENGTH, 0);
}

void pgGameScene::SendChatMsg()
{
	int length = GetWindowTextLength(hChat);
	
	if (length == 0 || length > MAX_CHAT_LENGTH)
	{
		return;
	}	
	TCHAR buf[MAX_CHAT_LENGTH];
	GetWindowText(hChat, buf, MAX_CHAT_LENGTH);
	PACKET_CHAT_MSG chatpac;
	chatpac.header.type = PACKET_TYPE_CHAT_MSG;
	chatpac.chatLength = length;
	memset(chatpac.msg, 0, sizeof(chatpac.msg));
	for (int i = 0; i < length; i++)
	{
		chatpac.msg[i] = buf[i];
	}
	chatpac.header.size = sizeof(PACKET_HEADER) + sizeof(int) * 2 + sizeof(wchar_t) * length;
	send(g_sock, (const char*)&chatpac, chatpac.header.size, 0);
	SetWindowText(hChat, TEXT(""));
}

void pgGameScene::OnInput(WPARAM wParam)
{

}