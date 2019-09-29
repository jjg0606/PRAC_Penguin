#pragma once

#define ROWS 5
#define COLS 5
#define MAX_LOBBY 10
#define MAX_CHAT_LENGTH 50
#define MAX_NAME_LENGTH 12

enum PACKET_TYPE
{
	PACKET_TYPE_SYSTEM,
	PACKET_TYPE_LOGIN_INFO,
	PACKET_TYPE_BLOCK_SETTING,
	PACKET_TYPE_LOBBY_INFO,
	PACKET_TYPE_LOBBY_IN,
	PACKET_TYPE_LOBBY_PLAYERS,
	PACKET_TYPE_GAME_READY,
	PACKET_TYPE_TURN_MSG,
	PACKET_TYPE_BREAK_BLOCK,
	PACKET_TYPE_CHAT_MSG,
};

enum SYSTEM_MSG
{
	SYSTEM_MSG_CLIENT_READY,
	SYSTEM_MSG_SERVER_READY,
};

enum TURN_COMMAND
{
	COMMAND_NOTHING,
	COMMAND_BREAK_WHITE,
	COMMAND_BREAK_BLUE,
	COMMAND_BREAK_BOTH,
	COMMAND_BREAK_NOTHING
};

#pragma pack(1)
typedef struct
{
	int type;
	int size;
}
PACKET_HEADER;

typedef struct
{
	PACKET_HEADER header;
	int system_msg;
}
PACKET_SYSTEM;

typedef struct
{
	PACKET_HEADER header;
	int avatar;
	wchar_t name[MAX_NAME_LENGTH];
}
PACKET_LOGIN_INFO;

typedef struct
{
	PACKET_HEADER header;
	int blockState[ROWS*COLS];
	int trunIdx;
	int command;
}
PACKET_BLOCK_SETTING;

typedef struct
{
	PACKET_HEADER header;
	int roomNum;
	bool isPlaying[MAX_LOBBY];
	int playerNum[MAX_LOBBY];
}
PACKET_LOBBY_INFO;

typedef struct
{
	PACKET_HEADER header;
	int index;
}
PACKET_LOBBY_IN;

typedef struct
{
	PACKET_HEADER header;
	int playerNum;
	int myNum;
	int avatar[4];
	bool isReady[4];
}
PACKET_LOBBY_PLAYERS;

typedef struct
{
	PACKET_HEADER header;
	int turnp;
	int command;
}
PACKET_TURN_MSG;

typedef struct
{
	PACKET_HEADER header;
	int nums;
	int idx[ROWS * COLS];
}
PACKET_BREAK_BLOCK;

typedef struct
{
	PACKET_HEADER header;
	int playerIdx;
	int chatLength;
	wchar_t msg[MAX_CHAT_LENGTH];
}
PACKET_CHAT_MSG;

#pragma pack()