//Emir Erbasan (humanova) 2018
#pragma once
#include "utils.h"

class SolHook
{

public:
	
	struct Bullet
	{
		int id;
		BYTE owner = 0;
		BYTE ownerWeapon = 0;
		bool active = 0;
		Vec2f pos = Vec2f();
		Vec2f vel = Vec2f();
	};

	struct Player
	{
		int id = 0;
		float health = 0;
		Vec2f pos = Vec2f();
		Vec2f vel = Vec2f();
	};

	struct Enemy
	{
		Player player;
	};

	struct Window
	{
		Vec2f WindowSize;
		float ScreenRatio;
		float CursorH;
		float CursorW;
		double Game2WindowRatioY;
		double Game2WindowRatioX;
		double Window2GameRatioY;
		double Window2GameRatioX;

		Vec2d CamCursorDiffGame;
		Vec2d CamCursorDiffWindow;
		double CamCursorRatioGame;
		double CamCursorRatioWindow;
	};

	struct gameVal
	{
		Vec2f playerPos;
		Vec2f playerVel;
		Vec2f cameraPos;
		Vec2f cursorPos;
		Vec2f mapCursorPos;
		int playerCount;
		int activeBulletNum;
		int activeSelfBulletNum;
		int enemyCount;
		int playerID;
		int aimbotID;
		bool aimbotting;
		int stickiedPlayerID;
		BYTE playerTeam;
		BYTE currentWeapon;
		float currentWeaponVel;
	};

	struct Settings
	{
		bool tele2cursor = 1;
		bool infiniteKnife = 1;
		bool betterBarret = 1;
		bool stabilizer = 1;
		bool tele2closestEnemy = 1;
		bool stick2player = 1;
		bool aimbot = 1;
		bool speedHack = 1;
		bool magicBullet = 1;
		int aimbotYOffset = -0;
		float aimbotCursorDistance = 75;
		float aimbotPlayerDistance = 635;
		double speedHackVal = 0.000007;

	}defaultSettings;

	struct Toggles
	{
		bool tele2cursor = 0;
		bool infiniteKnife = 0;
		bool betterBarret = 0;
		bool stabilizer = 0;
		bool tele2closestEnemy = 0;
		bool stick2player = 0;
		bool aimbot = 0;
		bool speedHack = 0;
		bool magicBullet = 0;
	};

	SolHook();
	SolHook(Settings launchSettings);
	void SetSettings(Settings newSettings);

	HANDLE GetSoldatHandle();
	Vec2f GetWindowSize();

	int GetPlayerID();
	Player GetPlayer(int p_id);
	int GetPlayerCount();
	Vec2f GetPlayerPos(int p_id);
	Vec2f GetCameraPos();
	Vec2f GetPlayerVel(int p_id);
	float GetPlayerHealth(int p_id);
	BYTE GetCurrentWeapon(int p_id);
	float GetCurrentWeaponVel();
	BYTE GetPlayerTeam(int p_id);

	Vec2f GetCursorPos();
	bool isScoreBoardOn();
	bool isPlayerOnline(int p_id);

	void RefreshEnemy();
	Enemy GetClosestEnemy();
	Enemy GetClosestEnemyCursor();
	void RefreshBullet();
	Bullet GetBullet(int bullet_id);
	void GetSelfBullet();
	
	void SetPlayerPos(int p_id, Vec2f pl_pos);
	void SetCameraPos(Vec2f cam_pos);
	void SetPlayerVel(int p_id, Vec2f pl_vel);
	void SetBulletPos(int bullet_id, Vec2f pos);
	void SetBulletVel(int bullet_id, Vec2f vel);
	void SetCursorPos(Vec2f cursor_pos);
	void SetCurrentWeapon(int p_id, BYTE weaponID);
	void SetBullet(int bullet_id, Vec2f pos, Vec2f vel);

	void Aimbot();
	void Aimbot(int p_id);
	void MagicBullet();
	void StabilizeCam(int specID);
	void Tele2Cursor();
	void Tele2Player(int p_id);
	void Tele2ClosestEnemy();
	void FixBarret();
	void SpeedHack(bool isRight);
	
	void RefreshVal();
	void CheckEvents();

	void DebugWinRatio();
	void PrintStatus();

	Settings settings;
	Toggles toggles;
	gameVal val;
	Window winset;
	Enemy enemyList[30];
	Bullet bulletList[50];
	Bullet selfBulletList[25];

private:

	float CalcDistance(Vec2f pos1, Vec2f pos2);
	void CalcWinset();

	Vec2f Window2Map();
	Vec2f Map2Cursor(Vec2f pos);

	void CheckToggles();

	void TypeRed(int type);
	void TypeGreen(int type);
	void TypeNormal();
	
	HANDLE game_handle;
};

