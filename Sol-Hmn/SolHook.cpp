#include "SolHook.h"

//Constructor without launch settings
SolHook::SolHook()
{
	SetSettings(defaultSettings);
	GetSoldatHandle();
	RefreshVal();
	CalcWinset();
}

//Constructor with launch settings
SolHook::SolHook(Settings launchSettings)
{
	SetSettings(launchSettings);
	GetSoldatHandle();
	RefreshVal();
	CalcWinset();
}

HANDLE SolHook::GetSoldatHandle()
{
	DWORD procId = GetProcId(L"Soldat.exe");
	HANDLE SolHandle = 0;
	if (!procId)
	{
		printf("Couldn't get Soldat PID...");
		Sleep(1000); exit(-1);
	}
	else
	{
		SolHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	}
	game_handle = SolHandle;
	return SolHandle;
}

Vec2 SolHook::GetWindowSize()
{
	Vec2 winSize;
	winSize.x = readMem<int>(game_handle, SoldatOffset::WindowWidth);
	winSize.y = readMem<int>(game_handle, SoldatOffset::WindowHeight);
	
	return winSize;
}


// ============== READ FUNCTIONS ===============

int SolHook::GetPlayerID()
{
	int p_id = readMem<int>(game_handle, SoldatOffset::camID);
	return p_id;
}

int SolHook::GetPlayerCount()
{
	int pl_count = readMem<int>(game_handle, SoldatOffset::playerCount);
	return pl_count;
}

Vec2 SolHook::GetPlayerPos(int p_id)
{
	Vec2 p_pos;
	p_pos.x = readMem<float>(game_handle, SoldatOffset::playerX + ((p_id - 1) * 0x8));
	p_pos.y = readMem<float>(game_handle, SoldatOffset::playerY + ((p_id - 1) * 0x8));
	return p_pos;
}

Vec2 SolHook::GetCameraPos()
{
	Vec2 cam_pos;
	cam_pos.x = readMem<float>(game_handle, SoldatOffset::cameraX);
	cam_pos.y = readMem<float>(game_handle, SoldatOffset::cameraY);
	return cam_pos;
}

Vec2 SolHook::GetPlayerVel(int p_id)
{
	Vec2 p_vel;
	p_vel.x = readMem<float>(game_handle, SoldatOffset::playerVelX + ((p_id - 1) * 0x8));
	p_vel.y = readMem<float>(game_handle, SoldatOffset::playerVelY + ((p_id - 1) * 0x8));
	return p_vel;
}

float SolHook::GetPlayerHealth(int p_id)
{
	float plHealth = readMem<float> (game_handle, SoldatOffset::playerHealth + ((p_id - 1) * playerOffset));
	return plHealth;
}

BYTE SolHook::GetPlayerTeam(int p_id)
{
	float plTeam = readMem<BYTE>(game_handle, SoldatOffset::playerTeam + ((p_id - 1) * playerOffset));
	return plTeam;
}

Vec2 SolHook::GetCursorPos()
{
	Vec2 c_pos;
	c_pos.x = readMem<float>(game_handle, SoldatOffset::playerCursorX);
	c_pos.y = readMem<float>(game_handle, SoldatOffset::playerCursorY);
	return c_pos;
}

BYTE SolHook::GetCurrentWeapon(int p_id)
{
	BYTE weaponID = readMem<BYTE>(game_handle, SoldatOffset::playerWeapon + ((p_id - 1) * playerOffset));
	return weaponID;
}

bool SolHook::isScoreBoardOn()
{
	bool scBoard = readMem<short>(game_handle, SoldatOffset::scoreToggle);
	return scBoard;
}

bool SolHook::isPlayerOnline(int p_id)
{
	bool isOnline = readMem<BYTE>(game_handle, SoldatOffset::playerOnline + ((p_id - 1) * playerOffset));
	return isOnline;
}

// ==============  WRITE FUNCTIONS ===============

void SolHook::SetPlayerPos(int p_id, Vec2 pl_pos)
{
	writeMem<float>(game_handle, SoldatOffset::playerX + ((p_id - 1) * 0x8), pl_pos.x);
	writeMem<float>(game_handle, SoldatOffset::playerY + ((p_id - 1) * 0x8), pl_pos.y);
}

void SolHook::SetCameraPos(Vec2 cam_pos)
{
	writeMem<float>(game_handle, SoldatOffset::cameraX, cam_pos.x);
	writeMem<float>(game_handle, SoldatOffset::cameraY, cam_pos.y);
}

void SolHook::SetPlayerVel(int p_id, Vec2 pl_vel)
{
	writeMem<float>(game_handle, SoldatOffset::playerX + ((p_id - 1) * 0x8), pl_vel.x);
	writeMem<float>(game_handle, SoldatOffset::playerY + ((p_id - 1) * 0x8), pl_vel.y);
}

void SolHook::SetCursorPos(Vec2 cursor_pos)
{
	writeMem<float>(game_handle, SoldatOffset::playerCursorX, cursor_pos.x);
	writeMem<float>(game_handle, SoldatOffset::playerCursorY, cursor_pos.y);
}

void SolHook::SetCurrentWeapon(int p_id, BYTE weaponID)
{
	writeMem<BYTE>(game_handle, SoldatOffset::playerWeapon + ((p_id - 1) * playerOffset), weaponID);
}

void SolHook::Aimbot(int p_id)
{
	Vec2 pPos = GetPlayerPos(p_id);
	Vec2 aimPos = Map2Cursor(pPos);

	SetCursorPos(aimPos);
}

void SolHook::Aimbot()
{
	Enemy enemy = GetClosestEnemyCursor();
	Vec2 aimPos = Map2Cursor(enemy.pos);
	aimPos.x += -1 * (GetPlayerVel(val.playerID).x - enemy.vel.x) * (1 / winset.Game2WindowRatioX);
	aimPos.y += -1 * (GetPlayerVel(val.playerID).y - enemy.vel.y) * (1 / winset.Game2WindowRatioY);
	if (CalcDistance(enemy.pos, val.mapCursorPos) < 1000)
	{
			SetCursorPos(aimPos);
	}
}

void SolHook::StabilizeCam(int specID)
{
	SetCameraPos(val.playerPos);
}

void SolHook::Tele2Cursor()
{
	SetPlayerPos(val.playerID, Window2Map());
}

void SolHook::Tele2Player(int p_id)
{
	SetPlayerPos(val.playerID, GetPlayerPos(p_id));
}

void SolHook::Tele2ClosestEnemy()
{
	Enemy closestEnemy = GetClosestEnemy();
	SetPlayerPos(val.playerID, closestEnemy.pos);
}

void SolHook::FixBarret()
{
	writeMem<int>(game_handle, SoldatOffset::barretBink, 0);
	writeMem<float>(game_handle, SoldatOffset::barretMovAcc, 0);
	writeMem<short>(game_handle, SoldatOffset::barretStartupTime, 0);
}

void SolHook::CheckEvents()
{
	CheckToggles();

	if (settings.tele2cursor && toggles.tele2cursor)
	{
		if (GetAsyncKeyState(VK_LSHIFT) & 1)
		{
			Tele2Cursor();
		}
	}

	if (settings.infiniteKnife && toggles.infiniteKnife)
	{
		if (val.currentWeapon == 255)
		{
			SetCurrentWeapon(val.playerID, 11);
		}
	}

	if (settings.betterBarret && toggles.betterBarret)
	{
		if (val.currentWeapon == 8)
		{
			if (isScoreBoardOn())
			{
				FixBarret();
			}
		}
	}

	if (settings.stabilizer && toggles.stabilizer)
	{
		StabilizeCam(val.playerID);
	}

	if (settings.tele2closestEnemy & toggles.tele2closestEnemy)
	{
		if (GetAsyncKeyState(VK_MENU) & 1)
		{
			Tele2ClosestEnemy();
		}
	}

	if (settings.stick2player & toggles.stick2player)
	{
		if ((GetAsyncKeyState(VK_LEFT) & 1) && (val.stickiedPlayerID - 1 > 0))
		{
			val.stickiedPlayerID -= 1;
		}
		if (GetAsyncKeyState(VK_RIGHT) & 1)
		{
			val.stickiedPlayerID += 1;
		}

		Tele2Player(val.stickiedPlayerID);
	}

	if (GetAsyncKeyState(VK_HOME) & 1)
	{
		PrintStatus();
	}
}

void SolHook::SetSettings(Settings newSettings)
{
	settings = newSettings;
}

void SolHook::RefreshVal()
{
	val.playerID = GetPlayerID();
	val.playerTeam = GetPlayerTeam(val.playerID);
	val.playerPos = GetPlayerPos(val.playerID);
	val.playerCount = GetPlayerCount();
	val.currentWeapon = GetCurrentWeapon(val.playerID);
	val.playerVel = GetPlayerVel(val.playerID);
	val.cursorPos = GetCursorPos();
	val.mapCursorPos = Window2Map();
	val.cameraPos = GetCameraPos();
}

void SolHook::RefreshEnemy()
{
	bool isOnline;
	Vec2 pos;
	Vec2 vel;
	float health;

	int pl = 1;
	int en = 0;
	do
	{
		if (isPlayerOnline(pl))
		{
			enemyList[pl] = emptyEnemy;

			if (!(GetPlayerTeam(pl) == val.playerTeam))
			{
				if (GetPlayerHealth(pl) > 0)
				{
					en += 1;

					Enemy enemy;
					enemy.id = pl;
					enemy.health = GetPlayerHealth(pl);
					enemy.pos = GetPlayerPos(pl);
					enemy.vel = GetPlayerVel(pl);

					enemyList[en] = enemy;
				}
			}
			pl += 1;
		}
		else
		{
			continue;
		}
	}while (pl <= val.playerCount);

	val.enemyCount = en;
}

void SolHook::DebugSomething()
{
	Vec2 camPosGame = val.cameraPos;
	Vec2 cursorPosGame = val.mapCursorPos;

	Vec2 camPosWindow;
	camPosWindow.x = winset.CursorW / 2;
	camPosWindow.y = winset.CursorH / 2;
	
	Vec2 cursorPosWindow = val.cursorPos;
	

	winset.CamCursorDiffGame.x = fabs(cursorPosGame.x - camPosGame.x);
	winset.CamCursorDiffGame.y = fabs(cursorPosGame.y - camPosGame.y);
	
	winset.CamCursorDiffWindow.x = fabs(cursorPosWindow.x - camPosWindow.x);
	winset.CamCursorDiffWindow.y = fabs(cursorPosWindow.y - camPosWindow.y);

	winset.CamCursorRatioGame = (winset.CamCursorDiffGame.x / winset.CamCursorDiffGame.y);
	winset.CamCursorRatioWindow = (winset.CamCursorDiffWindow.x / winset.CamCursorDiffWindow.y);
}


void SolHook::PrintStatus()
{
	system("cls");

	if (toggles.betterBarret && settings.betterBarret){TypeGreen(2);printf("1 - BetterBarret(F1) : ON\n");}
	else{ TypeRed(2); printf("1 - BetterBarret(F1) : OFF\n"); }
	if (toggles.tele2cursor && settings.tele2cursor) { TypeGreen(2); printf("2 - Tele2Cursor(F2 - LSHIFT) : ON\n"); }
	else { TypeRed(2); printf("2 - Tele2Cursor(F2 - LSHIFT) : OFF\n"); }
	if (toggles.tele2closestEnemy && settings.tele2closestEnemy) { TypeGreen(2); printf("3 - Tele2ClosestEnemy(F3 - LALT) : ON\n"); }
	else { TypeRed(2); printf("3 - Tele2ClosestEnemy(F3 - LALT) : OFF\n"); }
	if (toggles.infiniteKnife && settings.infiniteKnife) { TypeGreen(2); printf("4 - InfiniteKnife(F4) : ON\n"); }
	else { TypeRed(2); printf("4 - InfiniteKnife(F4) : OFF\n"); }
	if (toggles.stick2player && settings.stick2player) { TypeGreen(2); printf("5 - Stick2Player(F5) : ON\n"); }
	else { TypeRed(2); printf("5 - Stick2Player(F5) : OFF\n"); }
	if (toggles.stabilizer && settings.stabilizer) { TypeGreen(2); printf("6 - CamStabilizer(F6) : ON\n"); }
	else { TypeRed(2); printf("6 - CamStabilizer(F6) : OFF\n"); }
}

float SolHook::CalcDistance(Vec2 pos1, Vec2 pos2)
{
	float distance = sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y));
	return distance;
}

SolHook::Enemy SolHook::GetClosestEnemy()
{
	Enemy closestEnemy;
	if (val.enemyCount == 1)
	{
		closestEnemy = enemyList[1];
	}
	else 
	{
		for (int en = 1; en <= val.enemyCount; en++)
		{
			if (en == 1)
			{
				closestEnemy = enemyList[en];
			}
			else
			{
				if (CalcDistance(enemyList[en].pos, val.playerPos) < CalcDistance(closestEnemy.pos, val.playerPos))
				{
					closestEnemy = enemyList[en];
				}
			}
		}
	}

	return closestEnemy;
}

SolHook::Enemy SolHook::GetClosestEnemyCursor()
{
	Enemy closestEnemy;
	if (val.enemyCount == 1)
	{
		closestEnemy = enemyList[1];
	}
	else
	{
		for (int en = 1; en <= val.enemyCount; en++)
		{
			if (en == 1)
			{
				closestEnemy = enemyList[en];
			}
			else
			{
				if (CalcDistance(enemyList[en].pos, val.mapCursorPos) < CalcDistance(closestEnemy.pos, val.mapCursorPos))
				{
					closestEnemy = enemyList[en];
				}
			}
		}
	}

	return closestEnemy;
}

Vec2 SolHook::Map2Cursor(Vec2 pos)
{
	Vec2 camPos = val.cameraPos;
	Vec2 cursorPos;

	float gameDiffX = camPos.x - pos.x;
	float gameDiffY = camPos.y - pos.y;

	float deltaX = -1 * gameDiffX * (1 / winset.Game2WindowRatioX);
	float deltaY = -1 * gameDiffY * (1 / winset.Game2WindowRatioY);
	
	cursorPos.x = (winset.CursorW / 2) + deltaX;
	cursorPos.y = (winset.CursorH / 2) + deltaY;
	
	return cursorPos;
}

Vec2 SolHook::Window2Map()
{
	float x_dif = val.cameraPos.x - val.playerPos.x;
	float y_dif = val.cameraPos.y - val.playerPos.y;

	Vec2 cursorPos;
	cursorPos.x = val.cameraPos.x + x_dif;
	cursorPos.y = val.cameraPos.y + y_dif;

	return cursorPos;
}

void SolHook::CalcWinset()
{
	winset.WindowSize = GetWindowSize();
	winset.ScreenRatio = winset.WindowSize.x / winset.WindowSize.y;
	winset.CursorH = 480;
	winset.CursorW = round(winset.ScreenRatio * winset.CursorH);

	if (winset.ScreenRatio < 1.5)
	{
		winset.Game2WindowRatioY = 1.0204;
		winset.Game2WindowRatioX = 1.0204;

	}
	else if (winset.ScreenRatio > 1.5 && winset.ScreenRatio < 1.70)
	{
		winset.Game2WindowRatioY = 1.0217;
		winset.Game2WindowRatioX = 0.9188;
	}
	else if (winset.ScreenRatio > 1.70)
	{
		winset.Game2WindowRatioY = 1.1241;
		winset.Game2WindowRatioX = 0.5093;
	}
	
}

void SolHook::CheckToggles()
{
	if (GetAsyncKeyState(VK_F1) & 1)
	{
		toggles.betterBarret = !(toggles.betterBarret);
	}

	if (GetAsyncKeyState(VK_F2) & 1)
	{
		toggles.tele2cursor = !(toggles.tele2cursor);
	}

	if (GetAsyncKeyState(VK_F3) & 1)
	{
		toggles.tele2closestEnemy = !(toggles.tele2closestEnemy);
	}

	if (GetAsyncKeyState(VK_F4) & 1)
	{
		toggles.infiniteKnife = !(toggles.infiniteKnife);
	}

	if (GetAsyncKeyState(VK_F5) & 1)
	{
		val.stickiedPlayerID = 1;
		toggles.stick2player = !(toggles.stick2player);
	}

	if (GetAsyncKeyState(VK_F6) & 1)
	{
		toggles.stabilizer = !(toggles.stabilizer);
	}

}

void SolHook::TypeRed(int type)
{
	int colorValue = 4;
	if (type == 1) { type = colorValue; }
	else if (type != 1) { type = (type * 8) - (8 - colorValue); }
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), type);
}

void SolHook::TypeGreen(int type)
{
	int colorValue = 2;
	if (colorValue == 1) { type = colorValue; }
	else if (type != 1) { type = (type * 8) - (8 - colorValue); }
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), type);
}

void SolHook::TypeNormal() {
	int type = 2;
	int colorValue = 7;
	if (type == 1) { type = colorValue; }
	else if (type != 1) { type = (type * 8) - (8 - colorValue); }
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), type);
}