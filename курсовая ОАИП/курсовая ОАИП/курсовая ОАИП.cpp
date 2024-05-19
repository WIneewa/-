
#include <iostream>
#include <windows.h>
#include <fstream>
#include <ctime>
#include <string>
#include <locale>

using namespace std;

const int ROWS = 15;
const int COLS = 30;

struct GameState {
    char map[ROWS * COLS] = {};
    int playerX = 0, playerY = 0, monsterX1 = 0, monsterY1 = 0, monsterX2 = 0, monsterY2 = 0, doorX = 0, doorY = 0, treasureX = 0, treasureY = 0, keyX = 0, keyY = 0;
    bool keyFound = false;
    bool treasureTaken = false;
    int health = 3;
};

void initMap(GameState& game) {
    ifstream file("C:\\users\\Legion\\Desktop\\map.txt");
    string line;
    int row = 0;
    int monsterCount = 0; // Переменная для отслеживания количества монстров
    while (getline(file, line)) {
        for (int col = 0; col < COLS; ++col) {
            game.map[row * COLS + col] = line[col];
            if (line[col] == 'P') {
                game.playerX = row;
                game.playerY = col;
            }
            else if (line[col] == 'M') {
                if (monsterCount == 0) {
                    game.monsterX1 = row;
                    game.monsterY1 = col;
                }
                else {
                    game.monsterX2 = row;
                    game.monsterY2 = col;
                }
                monsterCount++;
            }
            else if (line[col] == 'K') {
                game.keyX = row;
                game.keyY = col;
            }
            else if (line[col] == 'T') {
                game.treasureX = row;
                game.treasureY = col;
            }
            else if (line[col] == 'D') {
                game.doorX = row;
                game.doorY = col;
            }
        }
        ++row;
    }
    file.close();
}


void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void printMap(GameState& game) {
    system("cls");
    setlocale(LC_ALL, "RU");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int index = i * COLS + j;
            if (game.map[index] != '\0') {
                switch (game.map[index]) {
                case 'P': // Игрок
                    setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                    cout << game.map[index];
                    break;
                case 'M': // Монстр
                    setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
                    cout << game.map[index];
                    break;
                case '#': // Стена
                    setColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                    cout << game.map[index];
                    break;
                case 'D': // Дверь
                    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                    cout << game.map[index];
                    break;
                case 'K': // Ключ
                case 'T': // Сундук
                    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                    cout << game.map[index];
                    break;
                default:
                    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                    cout << game.map[index];
                    break;
                }
            }
            else {
                cout << " ";
            }
        }
        cout << endl;
    }
    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    cout << (game.keyFound ? "Ключ был подобран." : "Ключ еще не подобран.") << endl;
    cout << (game.treasureTaken ? "Сундук был открыт." : "Сундук еще не открыт.") << endl;
    cout << "Текущее здоровье: " << game.health << endl;
}


void movePlayer(GameState& game) {
    if (game.health <= 0) {
        return;
    }

    char move;
    cout << "Введите ваше действие (w/a/s/d): ";
    cin >> move;


        int newPlayerX = game.playerX, newPlayerY = game.playerY;
    switch (move) {
    case 'w':
        newPlayerX = max(0, game.playerX - 1);
        break;
    case 'a':
        newPlayerY = max(0, game.playerY - 1);
        break;
    case 's':
        newPlayerX = min(ROWS - 1, game.playerX + 1);
        break;
    case 'd':
        newPlayerY = min(COLS - 1, game.playerY + 1);
        break;
    default:
        return;
    }

    if (game.map[newPlayerX * COLS + newPlayerY] != '#') {
        if (!((newPlayerX == game.doorX && newPlayerY == game.doorY) && !game.treasureTaken) && !((newPlayerX == game.treasureX && newPlayerY == game.treasureY) && !game.keyFound)) {
            game.map[game.playerX * COLS + game.playerY] = '.'; // Освобождаем старое место игрока
            game.playerX = newPlayerX;
            game.playerY = newPlayerY;
            game.map[game.playerX * COLS + game.playerY] = 'P'; // Обновляем позицию игрока на карте
        }
    }

    if (game.playerX == game.keyX && game.playerY == game.keyY) {
        game.keyFound = true;
    }

    if (game.keyFound && game.playerX == game.treasureX && game.playerY == game.treasureY) {
        game.treasureTaken = true;
    }

    printMap(game);
}


void moveMonster(GameState& game) {
    int move = rand() % 4;
    int newMonsterX1 = game.monsterX1, newMonsterY1 = game.monsterY1;
    int newMonsterX2 = game.monsterX2, newMonsterY2 = game.monsterY2;
    switch (move) {
    case 0:
        newMonsterX1 = max(0, game.monsterX1 - 1);
        newMonsterX2 = max(0, game.monsterX2 - 1);
        break;
    case 1:
        newMonsterY1 = max(0, game.monsterY1 - 1);
        newMonsterY2 = max(0, game.monsterY2 - 1);
        break;
    case 2:
        newMonsterX1 = min(ROWS - 1, game.monsterX1 + 1);
        newMonsterX2 = min(ROWS - 1, game.monsterX2 + 1);
        break;
    case 3:
        newMonsterY1 = min(COLS - 1, game.monsterY1 + 1);
        newMonsterY2 = min(COLS - 1, game.monsterY2 + 1);
        break;
    }

    if (game.map[newMonsterX1 * COLS + newMonsterY1] != '#' && game.map[newMonsterX1 * COLS + newMonsterY1] != 'K' && game.map[newMonsterX1 * COLS + newMonsterY1] != 'T' && game.map[newMonsterX1 * COLS + newMonsterY1] != 'D') {
        game.map[game.monsterX1 * COLS + game.monsterY1] = '.'; // Освобождаем старое место монстра
        game.monsterX1 = newMonsterX1;
        game.monsterY1 = newMonsterY1;
        game.map[game.monsterX1 * COLS + game.monsterY1] = 'M'; // Обновляем позицию монстра на карте
    }
    if (game.map[newMonsterX2 * COLS + newMonsterY2] != '#' && game.map[newMonsterX2 * COLS + newMonsterY2] != 'K' && game.map[newMonsterX2 * COLS + newMonsterY2] != 'T' && game.map[newMonsterX2 * COLS + newMonsterY2] != 'D') {
        game.map[game.monsterX2 * COLS + game.monsterY2] = '.'; // Освобождаем старое место монстра
        game.monsterX2 = newMonsterX2;
        game.monsterY2 = newMonsterY2;
        game.map[game.monsterX2 * COLS + game.monsterY2] = 'M'; // Обновляем позицию монстра на карте
    }

    printMap(game);
}

void fightMonster(GameState& game) {
    int index1 = game.playerX * COLS + game.playerY;
    int index2 = game.monsterX1 * COLS + game.monsterY1;
    int index3 = game.monsterX2 * COLS + game.monsterY2;
    if (game.playerX == game.monsterX1 && game.playerY == game.monsterY1 || game.playerX == game.monsterX2 && game.playerY == game.monsterY2) {
        game.health--;
        if (game.health <= 0) {
            cout << "Вы были пойманы монстром!\n";
            printMap(game);
            exit(0);
        }
    }
}

void checkExit(GameState& game) {
    int index = game.playerX * COLS + game.playerY;
    if (game.playerX == game.doorX && game.playerY == game.doorY && game.treasureTaken) {
        cout << "Вы успешно прошли лабиринт!\n";
        exit(0);
    }
} 

void changeLanguage() {
    cout << "Выберите язык:\n";
    cout << "1. Русский\n";
    cout << "2. Английский\n";
    cout << "Введите номер выбора: ";
    int choice;
    cin >> choice;
    switch (choice) {
    case 1:
        setlocale(LC_ALL, "Russian");
        break;
    case 2:
        setlocale(LC_ALL, "English");
        break;
    default:
        cout << "Неверный выбор. Используется русский язык по умолчанию.\n";
        setlocale(LC_ALL, "Russian");
        break;
    }
}

void displayMenu() {
    GameState game;
    int choice;
    initMap(game);
    do {
        system("cls");
        cout << "Меню игры:\n";
        cout << "1. Играть\n";
        cout << "2. Просмотреть правила\n";
        cout << "3. Настройки\n";
        cout << "4. Выход\n";
        cout << "Введите номер выбора: ";
        cin >> choice;

        switch (choice) {
        case 1:
            while (!game.treasureTaken) {
                printMap(game);
                movePlayer(game);
                moveMonster(game);
                fightMonster(game);
            }
            while (true) {
                printMap(game);
                movePlayer(game);
                moveMonster(game);
                fightMonster(game);
                checkExit(game);
            }
            break;
        case 2:
            system("cls");
            cout << "Правила игры:\n";
            cout << "1. Игрок может перемещаться по карте, используя клавиши 'w', 'a', 's', 'd'.\n";
            cout << "2. Монстры автоматически перемещаются по карте.\n";
            cout << "3. Если игрок встречается с монстром, его здоровье уменьшается на 1.\n";
            cout << "4. Игрок может подобрать ключ ('К') на карте. После того как ключ найден, игрок может открыть сундук ('С'), чтобы выиграть игру.\n";
            cout << "5. Выход из лабиринта ('Д') возможен только после того, как сундук был открыт. Если игрок выходит из лабиринта, игра завершается победой.\n";
            cout << "6. Игрок начинает игру с 3 единицами здоровья. Если здоровье достигает 0, игра завершается поражением.\n";
            cout << "7. Монстры автоматически перемещаются в случайном направлении. Они не могут перемещаться через стены, ключи или сундуки.\n";
            cout << "8. Игра продолжается до тех пор, пока игрок не найдет ключ, не откроет сундук и не выйдет из лабиринта, или пока его здоровье не достигнет 0.\n";
            cout << "Нажмите любую клавишу для возврата в главное меню...";
            cin.ignore();
            cin.get();
            displayMenu();
        case 3:
            changeLanguage();
            displayMenu();
        case 4:
            cout << "Выход из игры.\n";
            exit(0);
        default:
            cout << "Неверный выбор. Попробуйте снова.\n";
            Sleep(1000);
        }
    } while (choice != 1 && choice != 2 && choice != 3 && choice != 4);
}

int main() {
    setlocale(LC_ALL, "RU");
    displayMenu();
    return 0;
}