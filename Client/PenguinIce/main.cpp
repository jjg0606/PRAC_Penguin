#include "ProgramCore.h"
//#include "cmGameScene.h"
#include "pgGameScene.h"
using namespace std;

//Code here to Main
void ProgramCore::Main()
{
	ChangeScene(new pgGameScene());
}
