#pragma once
#include "utils.h"

struct Vec2
{
	float x = 0;
	float y = 0;
};


class SolHook
{

public:

	struct Window
	{
		Vec2 WindowSize;
		float ScreenRatio;
		float CursorH;
		float CursorW;
		float Game2WindowRatioY;
		float Game2WindowRatioX;

		Vec2 CamCursorDiffGame;
		Vec2 CamCursorDiffWindow;
		float CamCursorRatioGame;
		float CamCursorRatioWindow;
	};

	struct gameVal
	{
		Vec2 playerPos;
		Vec2 playerVel;
		Vec2 cameraPos;
		Vec2 cursorPos;
		Vec2 mapCursorPos;
		int playerCount;
		int enemyCount;
		int playerID;
		int stickiedPlayerID;
		BYTE playerTeam;
		BYTE currentWeapon;

		
	};

	struct Enemy
	{
		int id = 0;
		float health = 0;
		Vec2 pos = Vec2();
		Vec2 vel = Vec2();
	}emptyEnemy;

	struct Settings
	{
		bool tele2cursor = 1;
		bool infiniteKnife = 1;
		bool betterBarret = 1;
		bool stabilizer = 1;
		bool tele2closestEnemy = 1;
		bool stick2player = 1;

	}defaultSettings;

	struct Toggles
	{
		bool tele2cursor = 0;
		bool infiniteKnife = 0;
		bool betterBarret = 0;
		bool stabilizer = 0;
		bool tele2closestEnemy = 0;
		bool stick2player = 0;
	};

	SolHook();
	SolHook(Settings launchSettings);

	HANDLE GetSoldatHandle();
	Vec2 GetWindowSize();

	int GetPlayerID();
	int GetPlayerCount();
	Vec2 GetPlayerPos(int p_id);
	Vec2 GetCameraPos();
	Vec2 GetPlayerVel(int p_id);
	float GetPlayerHealth(int p_id);
	BYTE GetPlayerTeam(int p_id);

	Vec2 GetCursorPos();
	BYTE GetCurrentWeapon(int p_id);
	bool isScoreBoardOn();
	bool isPlayerOnline(int p_id);

	void SetPlayerPos(int p_id, Vec2 pl_pos);
	void SetCameraPos(Vec2 cam_pos);
	void SetPlayerVel(int p_id, Vec2 pl_vel);
	void SetCursorPos(Vec2 cursor_pos);
	void SetCurrentWeapon(int p_id, BYTE weaponID);

	void Aimbot(int p_id);
	void Aimbot();
	void StabilizeCam(int specID);
	void Tele2Cursor();
	void Tele2Player(int p_id);
	void Tele2ClosestEnemy();
	void FixBarret();
	
	void CheckEvents();
	void SetSettings(Settings newSettings);
	void RefreshVal();
	void RefreshEnemy();

	void DebugSomething();
	void PrintStatus();

	Settings settings;
	Toggles toggles;
	gameVal val;
	Window winset;
	Enemy enemyList[30];

	
	
private:

	float CalcDistance(Vec2 pos1, Vec2 pos2);
	Enemy GetClosestEnemy();
	Enemy GetClosestEnemyCursor();
	Vec2 Window2Map();
	void CalcWinset();
	
	void CheckToggles();
	HANDLE game_handle;
	Vec2 Map2Cursor(Vec2 pos);
	void TypeRed(int type);
	void TypeGreen(int type);
	void TypeNormal();

};

