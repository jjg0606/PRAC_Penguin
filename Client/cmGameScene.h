//#pragma once
//
//#include "Scene.h"
//#include "DrawObject.h"
//
//class cmGameScene
//	: virtual public Scene
//{
//	std::vector<std::pair<int, int>> posList;
//	DrawObject dro;
//	void ProcessPacket(char* szBuf, int len);
//public:
//	void DrawScreen(HDC hdc) override;
//	void OnInput(WPARAM wParam) override;
//	void Update() override;
//	void Release() override;
//	void Init() override;
//	void OnMouseClick(int x, int y, int E_MOUSE_BTN) override;
//	void OnMouseMove() override;
//	void OnSocketEvent(UINT iMessage, WPARAM wParam, LPARAM lParam) override;
//	~cmGameScene();
//};