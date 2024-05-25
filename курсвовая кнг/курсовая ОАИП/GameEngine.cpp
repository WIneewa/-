#include "GameEngine.h"

void GameEngine::StartEngine()
{
	InitMap();

	while (!ExitFlag)
	{
		system("cls");

		if (!IsGameStarted)
		{
			ShowMenu();
			continue;
		}

		while (PlayerHealth != 0)
		{
			system("cls");

			PrintMap();
			MovePlayer();

			if (IsPlayerMoved)
			{
				MoveMonster(1);
				MoveMonster(2);

				IsPlayerMoved = false;
			}

		}

		system("cls");
		cout << "�� �������!" << endl;
		IsGameStarted = false;

		cin.ignore();
		getchar();
	}
}

void GameEngine::InitMap()
{
	ifstream file;
	string line;
	int y = 0;
	int monsterCount = 0; // ���������� ��� ������������ ���������� ��������

	file.open(".\\map.txt");
	if (!file.is_open())
	{
		cout << "���� ����� �� ������" << endl;
		return;
	}

	while (getline(file, line))
	{
		for (int x = 0; x < COLS; ++x)
		{
			Map[y][x] = line[x];

			switch (line[x])
			{
			case 'P': PlayerPos = make_pair(x, y); break;
			case 'M': (monsterCount++ == 0 ? Monster1Pos : Monster2Pos) = make_pair(x, y); break;
			case 'K': KeyPos = make_pair(x, y); break;
			case 'T': TreasurePos = make_pair(x, y); break;
			case 'D': DoorPos = make_pair(x, y); break;
			default: break;
			}
		}

		++y;
	}

	StartPos = PlayerPos;
	file.close();
}

void GameEngine::SetColor(int color)
{
	SetConsoleTextAttribute(hConsole, color);
}

void GameEngine::PrintMap()
{
	for (int i = 0; i < ROWS; ++i)
	{
		for (int j = 0; j < COLS; ++j)
		{
			int color = 0;

			if (Map[i][j] == '\0')
			{
				cout << " ";
				continue;
			}

			switch (Map[i][j])
			{
			case 'P': color = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
			case 'M': color = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
			case '#': color = FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
			case 'D': color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
			case 'K':
			case 'T': color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
			default: color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
			}

			if (StartPos == make_pair(j, i) && PlayerPos != StartPos)
				Map[i][j] = 'S';

			SetColor(color);
			cout << Map[i][j];
		}

		cout << endl;
	}

	SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	cout << (KeyFound ? "���� ��� ��������." : "���� ��� �� ��������.") << endl;
	cout << (TreasureTaken ? "������ ��� ������." : "������ ��� �� ������.") << endl;
	cout << "������� ��������: " << PlayerHealth << endl;
}

void GameEngine::MovePlayer()
{
	char move;
	auto newPlayerPos = PlayerPos;

	if (PlayerHealth <= 0)
		return;

	cout << "������� ���� �������� (w/a/s/d): ";
	cin >> move;

	switch (move) {
	case 'w': newPlayerPos.second = max(0, PlayerPos.second - 1); break;
	case 'a': newPlayerPos.first = max(0, PlayerPos.first - 1); break;
	case 's': newPlayerPos.second = min(ROWS - 1, PlayerPos.second + 1); break;
	case 'd': newPlayerPos.first = min(COLS - 1, PlayerPos.first + 1); break;
	default: return;
	}

	if (Map[newPlayerPos.second][newPlayerPos.first] == '#')
		return;

	if (newPlayerPos == DoorPos && !TreasureTaken)
		return;

	if (newPlayerPos == TreasurePos && !KeyFound)
		return;

	IsPlayerMoved = true;

	if (newPlayerPos == Monster1Pos || newPlayerPos == Monster2Pos)
	{
		DamagePlayer();
		return;
	}

	if (newPlayerPos == KeyPos)
		KeyFound = true;

	if (newPlayerPos == TreasurePos && KeyFound)
		TreasureTaken = true;

	Map[PlayerPos.second][PlayerPos.first] = '.';

	PlayerPos = newPlayerPos;
	Map[PlayerPos.second][PlayerPos.first] = 'P';
}

void GameEngine::MoveMonster(int monsterNumber)
{
	auto& monsterPos = monsterNumber == 1 ? Monster1Pos : Monster2Pos;
	auto newMonsterPos = monsterPos;
	int mMoveX = -1 + (rand() % 3);
	int mMoveY = -1 + (rand() % 3);
	bool isMoved = false;

	if (Map[monsterPos.second][monsterPos.first + mMoveX] == '.')
	{
		newMonsterPos = make_pair(monsterPos.first + mMoveX, monsterPos.second);
		isMoved = true;
	}

	if (!isMoved && Map[monsterPos.second + mMoveY][monsterPos.first] == '.')
	{
		newMonsterPos = make_pair(monsterPos.first, monsterPos.second + mMoveY);
		isMoved = true;
	}

	//Revert move directions and try again
	if (!isMoved)
	{
		mMoveX = mMoveX == -1 ? 1 : -1;
		mMoveY = mMoveY == -1 ? 1 : -1;
	}

	if (Map[monsterPos.second][monsterPos.first + mMoveX] == '.')
	{
		newMonsterPos = make_pair(monsterPos.first + mMoveX, monsterPos.second);
		isMoved = true;
	}

	if (!isMoved && Map[monsterPos.second + mMoveY][monsterPos.first] == '.')
	{
		newMonsterPos = make_pair(monsterPos.first, monsterPos.second + mMoveY);
		isMoved = true;
	}

	//Monster can't move in any direction
	if (!isMoved)
		return;

	if (CheckMonsterPlayerCollision(newMonsterPos) && PlayerPos == StartPos)
		return;

	Map[monsterPos.second][monsterPos.first] = '.';

	monsterPos = newMonsterPos;
	Map[monsterPos.second][monsterPos.first] = 'M';
}

bool GameEngine::CheckMonsterPlayerCollision(const pair<int, int>& mPosition)
{
	if (mPosition == PlayerPos)
	{
		if (!IsPlayerDamaged)
			DamagePlayer();

		return true;
	}

	return false;
}

void GameEngine::DamagePlayer()
{
	PlayerHealth--;
	Map[PlayerPos.second][PlayerPos.first] = '.';
	Map[StartPos.second][StartPos.first] = 'P';

	PlayerPos = StartPos;
	IsPlayerDamaged = true;
}

bool GameEngine::CheckWinState()
{
	if (PlayerPos == DoorPos && TreasureTaken)
		return true;

	return false;
}

void GameEngine::ShowMenu()
{
	int choice;

	do
	{
		cout << "������� ����" << endl << endl;
		cout << "1. ������" << endl;
		cout << "2. ����������� �������" << endl;
		cout << "3. �����" << endl << endl;
		cout << "������� ����� ������: ";
		cin >> choice;

		switch (choice) {
		case 1: IsGameStarted = true; break;
		case 2: ShowHelp(); break;
		case 3: ExitFlag = true; break;
		default: break;
		}

	} while (choice < 0 || choice > 3);
}

void GameEngine::ShowHelp()
{
	system("cls");
	cout << "������� ����:\n";
	cout << "1. ����� ����� ������������ �� �����, ��������� ������� 'w', 'a', 's', 'd'.\n";
	cout << "2. ������� ������������� ������������ �� �����.\n";
	cout << "3. ���� ����� ����������� � ��������, ��� �������� ����������� �� 1.\n";
	cout << "4. ����� ����� ��������� ���� ('K') �� �����. ����� ���� ��� ���� ������, ����� ����� ������� ������ ('T'), ����� �������� ����.\n";
	cout << "5. ����� �� ��������� ('D') �������� ������ ����� ����, ��� ������ ��� ������. ���� ����� ������� �� ���������, ���� ����������� �������.\n";
	cout << "6. ����� �������� ���� � 3 ��������� ��������. ���� �������� ��������� 0, ���� ����������� ����������.\n";
	cout << "7. ������� ������������� ������������ � ��������� �����������. ��� �� ����� ������������ ����� �����, ����� ��� �������.\n";
	cout << "8. ���� ������������ �� ��� ���, ���� ����� �� ������ ����, �� ������� ������ � �� ������ �� ���������, ��� ���� ��� �������� �� ��������� 0.\n";
	cout << "������� ����� ������� ��� �������� � ������� ����...";

	cin.ignore();
	getchar();
}
