#pragma once
#include <map>
#include <vector>
#include <list>
#include "Scene.h"
#include "Image.h"
#include "EnumPgGameState.h"
#include "ImageRenderer.h"
#include "pgBtn.h"
#include "PACKET.h"

#define BUFSIZE 1024

enum BlockType
{
	NULL_BLOCK,
	WHITE_BLOCK,
	BLUE_BLOCK,
	PENGUIN_BLOCK
};

class pgGameScene
	: virtual public Scene
{
	std::map<int, Image*> imgMap;
	std::vector<std::vector<int>> blockMap;
	void SendInitMsg();
	void SendLobbbyInMsg(int index);
	void SendReadySignal();
	void SendBreakBlockMsg(int row,int col);
	void SendChatMsg();
	void SendLoginMsg();
	void SendLobbyChat();
	void UnSetLoginScreen();
	void LoadImgs();
	ImageRenderer render;
	pgGameState state = pgGameState::BEFORE_INIT;
	void ReadPacket(int length);
	// isplaying, num
	std::vector<std::pair<bool, int>> lobbyVec;
	std::vector<int> inGamePlayerAvatar;
	std::vector<bool> inGameReadyState;
	std::wstring nameArr[MAX_PLAYERS_IN_ROOM];
	std::vector<std::wstring> chatMsgVec;

	HWND hChat;
	HWND hName;
	HWND hLobbyChat;
	std::list<std::wstring> lobbyChatMsgList;
	const int idChat = 100;
	const int idName = 101;
	const int idLbyChat = 102;
	int selectedAvatar = 0;

	int turnIdx;
	int curCommand;
	int myIndex;
	bool isGameEnd = false;
	void SetLoginScreen();
	void SetLobbyScreen();
	void UnSetLobbyScreen();
	void DrawLogin(HDC hdc);
	void DrawInGame(HDC hdc);
	void DrawLobbyAvatar(HDC hdc);
	void DrawWatching(HDC hdc);
	void DrawInGameBlock(HDC hdc);
	void DrawInGameChatMsg(HDC hdc);
	void DrawInGameCommand(HDC hdc);
	pgBtn* readyBtn;
	pgBtn* exitBtn;
	pgBtn* refreshBtn;
	pgBtn* avatarBtn[4];
	pgBtn* confirmBtn;
	void DrawLobby(HDC hdc);
	char readBuf[BUFSIZE];
	int bufIdx = 0;
	void CutBuf(int length);
	void ClearReadBuf();
	void CreateChat();
	void LoginMouseHandle(int x, int y);
	void LobbyMouseHandle(int x, int y);
	void InGameMouseHandle(int x, int y);
	void WatchingMouseHandle(int x, int y);
public:
	void Update() override;
	void DrawScreen(HDC) override;
	void OnMouseClick(int x, int y, int BTN) override;
	void OnSocketEvent(UINT iMessage, WPARAM wParam, LPARAM lParam) override;
	void Init() override;
	void Release() override;
	void OnMenuMessage(WPARAM wParam) override;
	void OnInput(WPARAM wParam) override;
};