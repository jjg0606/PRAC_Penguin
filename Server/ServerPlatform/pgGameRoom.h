#pragma once
#include "PACKET.h"
#include <list>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include "pgGameUser.h"

enum BlockType
{
	NULL_BLOCK,
	WHITE_BLOCK,
	BLUE_BLOCK,
	PENGUIN_BLOCK
};

enum class pgGameState
{
	WAITING_PLAYER_JOIN,
	IN_GAME,
};

class pgGameRoom
{
private:
	//std::list<int> playerList;
	std::list<pgGameUser*> userList;
	pgGameState gameState = pgGameState::WAITING_PLAYER_JOIN;	
	std::vector<std::vector<int>> blockMap;
	
	std::shared_mutex listMutex;
	std::shared_mutex mapMutex;
	int cntBlock[3];
	int curCommand = COMMAND_NOTHING;
	int curTurnIdx = 0;
	void SyncLobbyPlayerInfo();
	void setBlockMap();
	void StartGame();
	void BreakBlock(int row, int col);
	bool chkIsFalling(int row, int col);
	bool isOutOfBound(int row, int col);
	void ProcessTurnEnd();
	void ProcessGameEnd();
	
public:
	bool JoinPlayer(pgGameUser* user);
	void DisconnectPlayer(int id);
	void chkStartCondition();
	std::vector<std::vector<int>>& getBlockMap();
	void getUsers(int* arr);
	void getRoomInfo(bool& isPlaying, int& cntPlayer);
	void getPlayerInput(int playerId, int row, int col);
	void broadCastChat(int userid, int length, wchar_t* buf);
};
