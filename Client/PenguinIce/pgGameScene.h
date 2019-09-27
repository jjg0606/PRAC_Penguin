#pragma once
#include <map>
#include <vector>
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
	void LoadImgs();
	ImageRenderer render;
	pgGameState state = pgGameState::BEFORE_INIT;
	void ReadPacket(int length);
	// isplaying, num
	std::vector<std::pair<bool, int>> lobbyVec;
	std::vector<int> inGamePlayerAvatar;
	std::vector<bool> inGameReadyState;
	std::vector<std::wstring> chatMsgVec;

	HWND hChat;
	const int idChat = 100;
	int turnIdx;
	int curCommand;
	int myIndex;
	bool isGameEnd = false;
	void DrawInGame(HDC hdc);
	void DrawLobbyAvatar(HDC hdc);
	pgBtn* readyBtn;
	pgBtn* exitBtn;
	pgBtn* refreshBtn;
	void DrawLobby(HDC hdc);
	char readBuf[BUFSIZE];
	int bufIdx = 0;
	void CutBuf(int length);
	void ClearReadBuf();
	void CreateChat();
	void LobbyMouseHandle(int x, int y);
	void InGameMouseHandle(int x, int y);
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