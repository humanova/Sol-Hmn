//Emir Erbasan (humanova) 2018
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

void SolHook::SetSettings(Settings newSettings)
{
	settings = newSettings;
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

// ============== Read Functions ===============

Vec2f SolHook::GetWindowSize()
{
	Vec2f winSize;
	winSize.x = readMem<int>(game_handle, SoldatOffset::WindowWidth);
	winSize.y = readMem<int>(game_handle, SoldatOffset::WindowHeight);
	
	return winSize;
}

int SolHook::GetPlayerID()
{
	int p_id = readMem<int>(game_handle, SoldatOffset::camID);
	return p_id;
}

SolHook::Player SolHook::GetPlayer(int p_id)
{
	Player player;
	player.id = p_id;
	player.health = GetPlayerHealth(p_id);
	player.pos = GetPlayerPos(p_id);
	player.vel = GetPlayerVel(p_id);

	return player;
}

int SolHook::GetPlayerCount()
{
	int pl_count = readMem<int>(game_handle, SoldatOffset::playerCount);
	return pl_count;
}

Vec2f SolHook::GetPlayerPos(int p_id)
{
	Vec2f p_pos;
	p_pos.x = readMem<float>(game_handle, SoldatOffset::playerX + ((p_id - 1) * 0x8));
	p_pos.y = readMem<float>(game_handle, SoldatOffset::playerY + ((p_id - 1) * 0x8));
	return p_pos;
}

Vec2f SolHook::GetCameraPos()
{
	Vec2f cam_pos;
	cam_pos.x = readMem<float>(game_handle, SoldatOffset::cameraX);
	cam_pos.y = readMem<float>(game_handle, SoldatOffset::cameraY);
	return cam_pos;
}

Vec2f SolHook::GetPlayerVel(int p_id)
{
	Vec2f p_vel;
	p_vel.x = readMem<float>(game_handle, SoldatOffset::playerVelX + ((p_id - 1) * 0x8));
	p_vel.y = readMem<float>(game_handle, SoldatOffset::playerVelY + ((p_id - 1) * 0x8));
	return p_vel;
}

float SolHook::GetPlayerHealth(int p_id)
{
	float plHealth = readMem<float> (game_handle, SoldatOffset::playerHealth + ((p_id - 1) * playerOffset));
	return plHealth;
}

BYTE SolHook::GetCurrentWeapon(int p_id)
{
	BYTE weaponID = readMem<BYTE>(game_handle, SoldatOffset::playerWeapon + ((p_id - 1) * playerOffset));
	return weaponID;
}

float SolHook::GetCurrentWeaponVel()
{
	float weaponVel = readMem<float>(game_handle, SoldatOffset::playerWeaponVel);
	return weaponVel;
}

BYTE SolHook::GetPlayerTeam(int p_id)
{
	BYTE plTeam = readMem<BYTE>(game_handle, SoldatOffset::playerTeam + ((p_id - 1) * playerOffset));
	return plTeam;
}

Vec2f SolHook::GetCursorPos()
{
	Vec2f c_pos;
	c_pos.x = readMem<float>(game_handle, SoldatOffset::playerCursorX);
	c_pos.y = readMem<float>(game_handle, SoldatOffset::playerCursorY);
	return c_pos;
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

void SolHook::RefreshEnemy()
{
	int pl = 1;
	int en = 0;
	do
	{
		if (isPlayerOnline(pl))
		{
			enemyList[pl] = Enemy();

			if (!(GetPlayerTeam(pl) == val.playerTeam))
			{
				if (GetPlayerHealth(pl) > 0)
				{
					en += 1;

					Enemy enemy;
					enemy.player.id = pl;
					enemy.player.health = GetPlayerHealth(pl);
					enemy.player.pos = GetPlayerPos(pl);
					enemy.player.vel = GetPlayerVel(pl);

					enemyList[en] = enemy;
				}
			}
			pl += 1;
		}
		else
		{
			continue;
		}
	} while (pl <= val.playerCount);

	val.enemyCount = en;
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
				if (CalcDistance(enemyList[en].player.pos, val.playerPos) <
					CalcDistance(closestEnemy.player.pos, val.playerPos))
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
				if (CalcDistance(enemyList[en].player.pos, val.mapCursorPos) < CalcDistance(closestEnemy.player.pos, val.mapCursorPos))
				{
					closestEnemy = enemyList[en];
				}
			}
		}
	}

	return closestEnemy;
}

void SolHook::RefreshBullet()
{
	Bullet bullet;
	int activeBulletNum = 0;

	for (int i = 0; i < val.playerCount * 7; i++)
	{
		bulletList[i] = Bullet();
		bullet = GetBullet(i);
		if (bullet.owner != 0 && bullet.active)
		{
			bulletList[activeBulletNum] = bullet;
			activeBulletNum++;
		}
	}
	val.activeBulletNum = activeBulletNum;
}

SolHook::Bullet SolHook::GetBullet(int bullet_id)
{
	Bullet bullet;
	bullet.id = bullet_id;
	bullet.owner = readMem<int>(game_handle, SoldatOffset::bulletOwner + ((bullet_id - 1) * bulletOffset));
	bullet.ownerWeapon = readMem<BYTE>(game_handle, SoldatOffset::bulletOwnerWeapon + ((bullet_id - 1) * bulletOffset));
	bullet.active = readMem<BYTE>(game_handle, SoldatOffset::bulletActive + ((bullet_id - 1) * bulletOffset));
	bullet.pos.x = readMem<float>(game_handle, SoldatOffset::bulletX + ((bullet_id - 1) * 0x8));
	bullet.pos.y = readMem<float>(game_handle, SoldatOffset::bulletY + ((bullet_id - 1) * 0x8));
	bullet.vel.x = readMem<float>(game_handle, SoldatOffset::bulletVelX + ((bullet_id - 1) * 0x8));
	bullet.vel.y = readMem<float>(game_handle, SoldatOffset::bulletVelY + ((bullet_id - 1) * 0x8));

	return bullet;
}

void SolHook::GetSelfBullet()
{
	int myBulletNum = 0;
	for (int i = 0; i < val.activeBulletNum; i++)
	{
		if (bulletList[i].owner == val.playerID)
		{
			selfBulletList[myBulletNum] = bulletList[i];
			myBulletNum++;
		}
	}
	val.activeSelfBulletNum = myBulletNum;
}

// ==============  Write Functions ===============

void SolHook::SetPlayerPos(int p_id, Vec2f pl_pos)
{
	writeMem<float>(game_handle, SoldatOffset::playerX + ((p_id - 1) * 0x8), pl_pos.x);
	writeMem<float>(game_handle, SoldatOffset::playerY + ((p_id - 1) * 0x8), pl_pos.y);
}

void SolHook::SetCameraPos(Vec2f cam_pos)
{
	writeMem<float>(game_handle, SoldatOffset::cameraX, cam_pos.x);
	writeMem<float>(game_handle, SoldatOffset::cameraY, cam_pos.y);
}

void SolHook::SetPlayerVel(int p_id, Vec2f pl_vel)
{
	writeMem<float>(game_handle, SoldatOffset::playerX + ((p_id - 1) * 0x8), pl_vel.x);
	writeMem<float>(game_handle, SoldatOffset::playerY + ((p_id - 1) * 0x8), pl_vel.y);
}

void SolHook::SetBulletPos(int bullet_id, Vec2f pos)
{
	writeMem<float>(game_handle, SoldatOffset::bulletX + ((bullet_id - 1) * bulletOffset), pos.x);
	writeMem<float>(game_handle, SoldatOffset::bulletY + ((bullet_id - 1) * bulletOffset), pos.y);
}

void SolHook::SetBulletVel(int bullet_id, Vec2f vel)
{
	writeMem<float>(game_handle, SoldatOffset::bulletVelX +((bullet_id - 1) * bulletOffset), vel.x);
	writeMem<float>(game_handle, SoldatOffset::bulletVelY +((bullet_id - 1) * bulletOffset), vel.y);
}

void SolHook::SetCursorPos(Vec2f cursor_pos)
{
	writeMem<float>(game_handle, SoldatOffset::playerCursorX, cursor_pos.x);
	writeMem<float>(game_handle, SoldatOffset::playerCursorY, cursor_pos.y);
}

void SolHook::SetCurrentWeapon(int p_id, BYTE weaponID)
{
	writeMem<BYTE>(game_handle, SoldatOffset::playerWeapon + ((p_id - 1) * playerOffset), weaponID);
}

void SolHook::SetBullet(int bullet_id, Vec2f pos, Vec2f vel)
{
	SetBulletPos(bullet_id, pos);
	SetBulletVel(bullet_id, vel);
}

// ======== Other Methods ==========

void SolHook::Aimbot(int p_id)
{
	Vec2f pPos = GetPlayerPos(p_id);
	Vec2f aimPos = Map2Cursor(pPos);
	aimPos.y += settings.aimbotYOffset;

	SetCursorPos(aimPos);
}

void SolHook::Aimbot()
{
	Enemy enemy;
	if (val.aimbotID != 0 && GetPlayerHealth(val.aimbotID) > 0)
	{
		enemy.player = GetPlayer(val.aimbotID);
	}
	else
	{
		enemy = GetClosestEnemyCursor();
	}

	float enemyCursorDistance = CalcDistance(enemy.player.pos, val.mapCursorPos);
	float enemyDistance = CalcDistance(enemy.player.pos, val.playerPos);

	if ((val.aimbotID != 0) && (enemyCursorDistance < settings.aimbotCursorDistance) || 
		((enemy.player.id != 0) &&
		(enemyCursorDistance < settings.aimbotCursorDistance) &&
		(enemyDistance < settings.aimbotPlayerDistance) &&
		(val.currentWeaponVel != 0.0f)))
	{
		float bulletTime = enemyDistance / val.currentWeaponVel;
		Vec2f aimPos = Map2Cursor(enemy.player.pos);
		aimPos.x += ((enemy.player.vel.x * bulletTime)) * winset.Game2WindowRatioX;
		aimPos.y += settings.aimbotYOffset + ((enemy.player.vel.y * bulletTime)) * winset.Game2WindowRatioY;

		//aimPos.x += -1 * (val.playerVel.x - (enemy.player.vel.x * bulletTime)) * (1 / winset.Game2WindowRatioX);
		//aimPos.y += -1 * (val.playerVel.x - (enemy.player.vel.y * bulletTime)) * (1 / winset.Game2WindowRatioY);

		SetCursorPos(aimPos);
		val.aimbotID = enemy.player.id;
		val.aimbotting = 1;
	}
	else
	{
		val.aimbotID = 0;
		val.aimbotting = 0;
	}
}

void SolHook::MagicBullet()
{
	Enemy enemy = GetClosestEnemy();
	if (enemy.player.id != 0)
	{
		for (int i = 0; i < val.activeSelfBulletNum; i++)
		{
			SetBullet(selfBulletList[i].id, enemy.player.pos, selfBulletList[i].vel);
		}
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
	SetPlayerPos(val.playerID, closestEnemy.player.pos);
}

void SolHook::FixBarret()
{
	if ((val.currentWeapon == 8) && (toggles.aimbot && settings.aimbot))
	{
		writeMem<short>(game_handle, SoldatOffset::barretStartupTime, 7);
	}
	else
	{
		writeMem<short>(game_handle, SoldatOffset::barretStartupTime, 0);
	}
	writeMem<int>(game_handle, SoldatOffset::barretBink, 0);
	writeMem<float>(game_handle, SoldatOffset::barretMovAcc, 0);
	
}

void SolHook::SpeedHack(bool isRight)
{
	//Tryna figure it out lol
}

void SolHook::RefreshVal()
{
	val.playerID = GetPlayerID();
	val.playerTeam = GetPlayerTeam(val.playerID);
	val.playerPos = GetPlayerPos(val.playerID);
	val.playerCount = GetPlayerCount();
	val.currentWeapon = GetCurrentWeapon(val.playerID);
	val.currentWeaponVel = GetCurrentWeaponVel();
	val.playerVel = GetPlayerVel(val.playerID);
	val.cursorPos = GetCursorPos();
	val.mapCursorPos = Window2Map();
	val.cameraPos = GetCameraPos();

	if (settings.magicBullet && toggles.magicBullet)
	{
		RefreshBullet();
		GetSelfBullet();
	}

	if ((settings.aimbot && toggles.aimbot) ||
		(settings.stick2player && toggles.stick2player) ||
		(settings.tele2closestEnemy && toggles.tele2closestEnemy) ||
		(settings.magicBullet && toggles.magicBullet))
	{
		RefreshEnemy();
	}
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

	if (settings.aimbot & toggles.aimbot)
	{
		if (GetKeyState(VK_LBUTTON) < 0)
		{
			Aimbot();
		}
		else
		{
			val.aimbotID = 0;
			val.aimbotting = 0;
		}
	}

	if (settings.speedHack & toggles.speedHack)
	{
		//Left (negative way)
		if (GetAsyncKeyState(0x41) & 1)
		{
			SpeedHack(0);
		}

		//Right (positive way)
		if (GetAsyncKeyState(0x44) & 1)
		{
			SpeedHack(1);
		}
	}

	if (settings.magicBullet & toggles.magicBullet)
	{
		MagicBullet();
	}

	if (GetAsyncKeyState(VK_HOME) & 1)
	{
		PrintStatus();
	}
}

void SolHook::DebugWinRatio()
{
	Vec2f camPosGame = val.cameraPos;
	Vec2f cursorPosGame = val.mapCursorPos;

	Vec2f camPosWindow;
	camPosWindow.x = winset.CursorW / 2;
	camPosWindow.y = winset.CursorH / 2;
	
	Vec2f cursorPosWindow = val.cursorPos;
	

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
	if (toggles.aimbot && settings.aimbot) { TypeGreen(2); printf("6 - Aimbot(F6) : ON\n"); }
	else { TypeRed(2); printf("6 - Aimbot(F6) : OFF\n"); }
	if (toggles.speedHack && settings.speedHack) { TypeGreen(2); printf("7 - SpeedHack(F7) : ON\n"); }
	else { TypeRed(2); printf("7 - SpeedHack(F7) : OFF\n"); }
	if (toggles.magicBullet && settings.magicBullet) { TypeGreen(2); printf("8 - MagicBullet(offline)(F8) : ON\n"); }
	else { TypeRed(2); printf("8 - MagicBullet(offline)(F8) : OFF\n"); }
	if (toggles.stabilizer && settings.stabilizer) { TypeGreen(2); printf("9 - CamStabilizer(F9) : ON\n"); }
	else { TypeRed(2); printf("9 - CamStabilizer(F9) : OFF\n"); }
}

float SolHook::CalcDistance(Vec2f pos1, Vec2f pos2)
{
	float distance = sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y));
	return distance;
}

void SolHook::CalcWinset()
{
	winset.WindowSize = GetWindowSize();
	winset.ScreenRatio = winset.WindowSize.x / winset.WindowSize.y;
	winset.CursorH = 480;
	winset.CursorW = round(winset.ScreenRatio * winset.CursorH);

	if (winset.ScreenRatio < 1.5)
	{
		winset.Game2WindowRatioY = 1.0204081217448;
		winset.Game2WindowRatioX = 1.0204078674316;

		winset.Window2GameRatioY = 1.0 / winset.Game2WindowRatioX;
		winset.Window2GameRatioX = 1.0 / winset.Game2WindowRatioX;
	}
	else if (winset.ScreenRatio > 1.5 && winset.ScreenRatio < 1.70)
	{
		winset.Game2WindowRatioY = 1.0217;
		winset.Game2WindowRatioX = 0.9188;

		winset.Window2GameRatioY = 1.0 / winset.Game2WindowRatioX;
		winset.Window2GameRatioX = 1.0 / winset.Game2WindowRatioX;
	}
	else if (winset.ScreenRatio > 1.70)
	{
		winset.Game2WindowRatioY = 1.1241;
		winset.Game2WindowRatioX = 0.5093;

		winset.Window2GameRatioY = 1.0 / winset.Game2WindowRatioX;
		winset.Window2GameRatioX = 1.0 / winset.Game2WindowRatioX;
	}

}

Vec2f SolHook::Window2Map()
{
	float x_dif = val.cameraPos.x - val.playerPos.x;
	float y_dif = val.cameraPos.y - val.playerPos.y;

	Vec2f cursorPos;
	cursorPos.x = val.cameraPos.x + x_dif;
	cursorPos.y = val.cameraPos.y + y_dif;

	return cursorPos;
}

Vec2f SolHook::Map2Cursor(Vec2f pos)
{
	Vec2f camPos = val.cameraPos;
	Vec2f cursorPos;

	float gameDiffX = camPos.x - pos.x;
	float gameDiffY = camPos.y - pos.y;

	float deltaX = -1 * gameDiffX * (1 / winset.Game2WindowRatioX);
	float deltaY = -1 * gameDiffY * (1 / winset.Game2WindowRatioY);
	
	cursorPos.x = (winset.CursorW / 2) + deltaX;
	cursorPos.y = (winset.CursorH / 2) + deltaY;
	
	return cursorPos;
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
		toggles.aimbot = !(toggles.aimbot);
	}

	if (GetAsyncKeyState(VK_F7) & 1)
	{
		toggles.speedHack = !(toggles.speedHack);
	}

	if (GetAsyncKeyState(VK_F8) & 1)
	{
		toggles.magicBullet = !(toggles.magicBullet);
	}

	if (GetAsyncKeyState(VK_F9) & 1)
	{
		toggles.stabilizer = !(toggles.stabilizer);
	}

}

// ========== Console Stuff ===========

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
	type = (type * 8) - 1;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), type);
}