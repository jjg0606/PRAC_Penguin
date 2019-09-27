#include "pgGameRoom.h"
#include <shared_mutex>

bool pgGameRoom::JoinPlayer(pgGameUser* user)
{
	std::unique_lock<std::shared_mutex> listLock(listMutex);

	if (userList.size() < 4 && gameState == pgGameState::WAITING_PLAYER_JOIN)
	{
		userList.push_back(user);
		listLock.unlock();
		SyncLobbyPlayerInfo();
		return true;
	}
	else
	{
		return false;
	}

	
}

void pgGameRoom::DisconnectPlayer(int id)
{
	std::unique_lock<std::shared_mutex> listLock(listMutex);
	// todo 
	// in gameplaying state
	for (auto iter = userList.begin(); iter != userList.end(); )
	{
		if ((*iter)->ID == id)
		{
			iter = userList.erase(iter);
			continue;
		}
		if (gameState == pgGameState::IN_GAME)
		{
			(*iter)->isReady = false;
		}
		iter++;
	}
	if (gameState == pgGameState::IN_GAME)
	{
		gameState = pgGameState::WAITING_PLAYER_JOIN;
	}
	
	listLock.unlock();
	SyncLobbyPlayerInfo();
}

void pgGameRoom::setBlockMap()
{
	std::unique_lock<std::shared_mutex> mapLock(mapMutex);
	memset(cntBlock, 0, sizeof(cntBlock));

	if (blockMap.empty())
	{
		blockMap.resize(ROWS);
		for (int r = 0; r < ROWS; r++)
		{
			blockMap[r].resize(COLS);
		}
	}

	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			int state = 1 + (rand() % 2);
			blockMap[r][c] = state;
			cntBlock[state]++;
		}
	}
	blockMap[ROWS / 2][COLS / 2] = PENGUIN_BLOCK;

	mapLock.unlock();
}

std::vector<std::vector<int>>& pgGameRoom::getBlockMap()
{
	std::shared_lock<std::shared_mutex> mapLock(mapMutex);
	return blockMap;
}

void pgGameRoom::getUsers(int* arr)
{
	std::shared_lock<std::shared_mutex> listLock(listMutex);

	for (auto iter = userList.begin(); iter != userList.end(); iter++)
	{
		*(arr++) = (*iter)->ID;
	}

	listLock.unlock();
}

void pgGameRoom::getRoomInfo(bool& isPlaying, int& cntPlayer)
{
	isPlaying = gameState == pgGameState::IN_GAME;
	
	std::shared_lock<std::shared_mutex> listLock(listMutex);
	cntPlayer = userList.size();

	listLock.unlock();
}

void pgGameRoom::getPlayerInput(int playerId, int row, int col)
{
	int index = 0;
	auto iter = userList.begin();
	for (; iter != userList.end(); iter++)
	{
		if (index++ == curTurnIdx)
		{
			break;
		}
	}

	if ((*iter)->ID != playerId)
	{
		return;
	}

	if (curCommand == COMMAND_BREAK_NOTHING)
	{
		ProcessTurnEnd();
		return;
	}

	if (row >= ROWS || row < 0 || col >= COLS || col < 0)
	{
		return;
	}
	
	std::shared_lock<std::shared_mutex> mapLock(mapMutex);

	if (blockMap[row][col] == NULL_BLOCK || blockMap[row][col] == PENGUIN_BLOCK)
	{
		return;
	}

	if (curCommand == COMMAND_BREAK_BLUE && blockMap[row][col] == WHITE_BLOCK)
	{
		return;
	}

	if (curCommand == COMMAND_BREAK_WHITE && blockMap[row][col] == BLUE_BLOCK)
	{
		return;
	}

	mapLock.unlock();

	BreakBlock(row, col);
}

void pgGameRoom::SyncLobbyPlayerInfo()
{
	pgGameUser* uList[4];
	int playerNum;
	std::shared_lock<std::shared_mutex> listLock(listMutex);

	playerNum = userList.size();
	int index = 0;
	for (auto iter = userList.begin(); iter != userList.end(); iter++)
	{
		uList[index++] = *iter;
	}

	listLock.unlock();

	for (auto iter = userList.begin(); iter != userList.end(); iter++)
	{
		(*iter)->SendLobbyPlayers(playerNum, uList);
	}
}

void pgGameRoom::chkStartCondition()
{
	std::shared_lock<std::shared_mutex> listLock(listMutex);

	bool isAllready = true;
	for (auto iter = userList.begin(); iter != userList.end(); iter++)
	{
		if ((*iter)->isReady == false)
		{
			isAllready = false;
			break;
		}
	}
	

	listLock.unlock();

	if (isAllready && userList.size() > 1)
	{
		// todo
		StartGame();	
	}
	else
	{
		SyncLobbyPlayerInfo();
	}
}

void pgGameRoom::StartGame()
{
	setBlockMap();
	gameState = pgGameState::IN_GAME;
	curTurnIdx = 0;
	
	std::shared_lock<std::shared_mutex> listLock(listMutex);
	int turnIdx = rand() % userList.size();
	int command = (rand() % COMMAND_BREAK_NOTHING) + 1;
	this->curTurnIdx = turnIdx;
	this->curCommand = command;

	for (auto iter = userList.begin(); iter != userList.end(); iter++)
	{
		(*iter)->SendBlockMap(blockMap,turnIdx,command);		
	}
}

void pgGameRoom::BreakBlock(int row, int col)
{
	static int dr[4] = {0,0,1,-1};
	static int dc[4] = {1,-1,0,0};

	int breakNum = 1;
	int breakArr[ROWS*COLS];
	memset(breakArr, 0, sizeof(breakArr));
	breakArr[0] = row * COLS + col;

	bool visited[ROWS][COLS];
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			visited[r][c] = false;
		}
	}

	visited[row][col] = true;

	std::list<std::pair<int, int>> bfsList;
	bfsList.push_back({ row,col });

	std::unique_lock<std::shared_mutex> mapLock(mapMutex);
	cntBlock[blockMap[row][col]]--;
	blockMap[row][col] = NULL_BLOCK;

	while (!bfsList.empty())
	{
		int curRow = bfsList.front().first;
		int curCol = bfsList.front().second;
		bfsList.pop_front();
		// check 4 way
		if (blockMap[curRow][curCol] != NULL_BLOCK && chkIsFalling(curRow, curCol))
		{
			cntBlock[blockMap[curRow][curCol]]--;
			blockMap[curRow][curCol] = NULL_BLOCK;
			breakArr[breakNum++] = curRow * COLS + curCol;
		}
		if (blockMap[curRow][curCol] == NULL_BLOCK)
		{
			for (int d = 0; d < 4; d++)
			{
				int nextRow = curRow + dr[d];
				int nextCol = curCol + dc[d];
				if (isOutOfBound(nextRow, nextCol) || visited[nextRow][nextCol])
				{
					continue;
				}

				visited[nextRow][nextCol] = true;
				bfsList.push_back({ nextRow,nextCol });
			}
		}
	}

	bool isGameWin = blockMap[ROWS / 2][COLS / 2] == NULL_BLOCK;
	mapLock.unlock();

	std::shared_lock<std::shared_mutex> listLock(listMutex);	
	for (auto iter = userList.begin(); iter != userList.end(); iter++)
	{
		(*iter)->SendBreakInfo(breakNum, breakArr);
	}
	listLock.unlock();

	if (isGameWin)
	{
		ProcessGameEnd();
	}
	else
	{
		ProcessTurnEnd();
	}
	
}

bool pgGameRoom::chkIsFalling(int row, int col)
{
	static int dr[4] = { 0,0,1,-1 };
	static int dc[4] = { 1,-1,0,0 };

	bool isConnected[4];

	for (int i = 0; i < 4; i++)
	{
		int nextRow = row + dr[i];
		int nextCol = col + dc[i];
		if (isOutOfBound(nextRow, nextCol) || blockMap[nextRow][nextCol] != NULL_BLOCK)
		{
			isConnected[i] = true;
		}
		else
		{
			isConnected[i] = false;
		}
	}

	if (isConnected[0] == true && isConnected[1] == true)
	{
		return false;
	}
	if (isConnected[2] == true && isConnected[3] == true)
	{
		return false;
	}
	return true;
}

bool pgGameRoom::isOutOfBound(int row, int col)
{
	if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
	{
		return true;
	}
	return false;
}

void pgGameRoom::ProcessTurnEnd()
{
	Sleep(100);
	curTurnIdx = (curTurnIdx + 1) % userList.size();
	curCommand = (rand() % COMMAND_BREAK_NOTHING) + 1;
	if (cntBlock[curCommand] == 0)
	{
		curCommand = COMMAND_BREAK_BOTH;
	}
	std::shared_lock<std::shared_mutex> listLock(listMutex);
	for (auto iter = userList.begin(); iter != userList.end(); iter++)
	{
		(*iter)->SendTurnInfo(curTurnIdx, curCommand);
	}
}

void pgGameRoom::ProcessGameEnd()
{
	Sleep(6000);
	gameState = pgGameState::WAITING_PLAYER_JOIN;
	std::unique_lock<std::shared_mutex> ulList(listMutex);
	for (auto iter = userList.begin(); iter != userList.end(); iter++)
	{
		(*iter)->OnGameEnd();
	}
	ulList.unlock();
	SyncLobbyPlayerInfo();
}

void pgGameRoom::broadCastChat(int userid, int length, wchar_t* buf)
{
	std::shared_lock<std::shared_mutex> listLock(listMutex);
	int useridx = 0;
	for (auto iter = userList.begin(); iter != userList.end(); iter++)
	{
		if ((*iter)->ID == userid)
		{
			break;
		}
		useridx++;
	}
	for (auto iter = userList.begin(); iter != userList.end(); iter++)
	{
		(*iter)->SendChatMsgTo(useridx, length, buf);
	}
}