#include "cmGameRoomMgr.h"

using namespace std;

cmGameRoomMgr cmGameRoomMgr::instance;

int cmGameRoomMgr::getRoomNum()
{
	if (RoomVertex.empty())
	{
		RoomVertex.resize(1);
	}
	return 0;
}

int cmGameRoomMgr::getPlayerNum()
{
	return playerNum++;
}

void cmGameRoomMgr::getVertexFrom(int room, int playerNum,int x,int y)
{
	if (playerNum == 0)
	{
		RoomVertex[0].push_back(make_pair(x, y));
	}
}

bool cmGameRoomMgr::Syncronize(int& x, int& y,int room, int vertexNum)
{
	if (vertexNum >= RoomVertex[room].size())
	{
		return false;
	}
	else
	{
		x = RoomVertex[room][vertexNum].first;
		y = RoomVertex[room][vertexNum].second;
		return true;
	}
}