#pragma once

enum class pgUserState
{
	BEFORE_READY_SIGNAL,
	INIT,
	IN_LOBBY,
	READY_FOR_GAME,
	IN_GAME,
};