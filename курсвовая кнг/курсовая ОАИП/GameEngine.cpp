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
		cout << "Вы погибли!" << endl;
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
	int monsterCount = 0; // Переменная для отслеживания количества монстров

	file.open(".\\map.txt");
	if (!file.is_open())
	{
		cout << "Файл карты не открыт" << endl;
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
	cout << (KeyFound ? "Ключ был подобран." : "Ключ еще не подобран.") << endl;
	cout << (TreasureTaken ? "Сундук был открыт." : "Сундук еще не открыт.") << endl;
	cout << "Текущее здоровье: " << PlayerHealth << endl;
}

void GameEngine::MovePlayer()
{
	char move;
	auto newPlayerPos = PlayerPos;

	if (PlayerHealth <= 0)
		return;

	cout << "Введите ваше действие (w/a/s/d): ";
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
		cout << "Главное меню" << endl << endl;
		cout << "1. Играть" << endl;
		cout << "2. Просмотреть правила" << endl;
		cout << "3. Выход" << endl << endl;
		cout << "Введите номер пункта: ";
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
	cout << "Правила игры:\n";
	cout << "1. Игрок может перемещаться по карте, используя клавиши 'w', 'a', 's', 'd'.\n";
	cout << "2. Монстры автоматически перемещаются по карте.\n";
	cout << "3. Если игрок встречается с монстром, его здоровье уменьшается на 1.\n";
	cout << "4. Игрок может подобрать ключ ('K') на карте. После того как ключ найден, игрок может открыть сундук ('T'), чтобы выиграть игру.\n";
	cout << "5. Выход из лабиринта ('D') возможен только после того, как сундук был открыт. Если игрок выходит из лабиринта, игра завершается победой.\n";
	cout << "6. Игрок начинает игру с 3 единицами здоровья. Если здоровье достигает 0, игра завершается поражением.\n";
	cout << "7. Монстры автоматически перемещаются в случайном направлении. Они не могут перемещаться через стены, ключи или сундуки.\n";
	cout << "8. Игра продолжается до тех пор, пока игрок не найдет ключ, не откроет сундук и не выйдет из лабиринта, или пока его здоровье не достигнет 0.\n";
	cout << "Нажмите любую клавишу для возврата в главное меню...";

	cin.ignore();
	getchar();
}
