//#include "cmGameScene.h"
//#include "PACKET.h"
//
//#define BUFSIZE 512
//
//using namespace std;
//
//void cmGameScene::DrawScreen(HDC hdc)
//{
//	FillRect(hdc, &getWinRect(),(HBRUSH)GetStockObject(WHITE_BRUSH));
//	dro.Render(hdc);
//}
//
//void cmGameScene::OnInput(WPARAM wParam)
//{
//
//}
//
//void cmGameScene::Update()
//{
//	/*dro.Update();*/
//	Invalidate();
//}
//
//void cmGameScene::Release()
//{
//
//}
//
//void cmGameScene::Init()
//{
//	ProgramCore::instance.SetUpdateIntersec(0);
//	Invalidate();
//}
//
//void cmGameScene::OnMouseClick(int x, int y, int E_MOUSE_BTN)
//{
//
//}
//
//cmGameScene::~cmGameScene()
//{
//
//}
//
//void cmGameScene::OnMouseMove()
//{
//	dro.Update();
//}
//
//void cmGameScene::OnSocketEvent(UINT iMessage, WPARAM wParam, LPARAM lParam)
//{
//	int addrlen = 0;
//	int retval = 0;
//
//	if (WSAGETSELECTERROR(lParam))
//	{
//		int err_code = WSAGETSELECTERROR(lParam);
//		return;
//	}
//
//	switch (WSAGETSELECTEVENT(lParam))
//	{
//	case FD_READ:
//	{
//		char szBuf[BUFSIZE];
//
//		retval = recv(wParam, szBuf, BUFSIZE, 0);
//		if (retval == SOCKET_ERROR)
//		{
//			if (WSAGetLastError() != WSAEWOULDBLOCK)
//			{
//				return;
//			}
//		}
//
//		ProcessPacket(szBuf, retval);
//	}
//	break;
//	case FD_CLOSE:
//		closesocket(wParam);
//		break;
//	}
//}
//
//void cmGameScene::ProcessPacket(char* szBuf, int len)
//{
//	PACKET_HEADER header;
//	memcpy(&header, szBuf, sizeof(header));
//
//	switch (header.type)
//	{
//	case SEND_POS:
//	{
//		struct PACKET_POINT getpos;
//		memcpy(&getpos, szBuf, header.size);
//		for (int i = 0; i < getpos.num; i++)
//		{
//			dro.PushPoint(getpos.xarr[i], getpos.yarr[i]);
//		}
//	}
//		break;
//	}
//}