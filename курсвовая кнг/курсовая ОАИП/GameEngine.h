#include <fstream>
#include <string>
#include <windows.h>
#include <iostream>
#include <vector>

using namespace std;

struct GameEngine
{
	const int ROWS = 15;
	const int COLS = 30;

	char Map[15][30];
	pair<int, int> StartPos = { 0, 0 };
	pair<int, int> PlayerPos = { 0, 0 };
	pair<int, int> Monster1Pos = { 0, 0 };
	pair<int, int> Monster2Pos = { 0, 0 };
	pair<int, int> DoorPos = { 0, 0 };
	pair<int, int> TreasurePos = { 0, 0 };
	pair<int, int> KeyPos = { 0, 0 };
	bool KeyFound = false;
	bool TreasureTaken = false;
	bool IsPlayerDamaged = false;
	int PlayerHealth = 3;
	bool IsPlayerMoved = false;
	bool IsGameStarted = false;
	bool ExitFlag = false;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	void StartEngine();

	void InitMap();
	void SetColor(int color);
	void PrintMap();
	void MovePlayer();
	void MoveMonster(int monsterNumber);
	bool CheckMonsterPlayerCollision(const pair<int, int>& mPosition);
	void DamagePlayer();
	bool CheckWinState();
	void ShowMenu();
	void ShowHelp();
};
