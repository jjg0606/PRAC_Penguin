#pragma once
#include <vector>
#include <utility>

class cmGameRoomMgr
{
private:
	cmGameRoomMgr() = default;
	int roomSize = 0;
	int playerNum;
	std::vector<std::vector<std::pair<int, int>>> RoomVertex;
public:
	static cmGameRoomMgr instance;
	int getRoomNum();
	int getPlayerNum();
	void getVertexFrom(int room, int playerNum,int x,int y);
	bool Syncronize(int& x, int& y, int room, int vertexNum);
};
