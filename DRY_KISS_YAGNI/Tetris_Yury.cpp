#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

enum TurnDirection
{
    Clockwise,
    CounterClockwise
};

// DRY: ANSI-формат пишется один раз, все цвета идут через эту функцию.
string ansi(int colorNumber)
{
    return "\033[48;5;" + to_string(colorNumber) + "m  \033[m";
}

// DRY: вместо switch на 10 кейсов с printf, таблица имя в номер.
map<string, int> colorNumbers = {
    {"hidden", 238},
    {"empty", -1},
    {"active", 255},
    {"violet", 105},
    {"green", 10},
    {"red", 160},
    {"orange", 208},
    {"blue", 75},
    {"cyan", 123},
    {"yellow", 220},
    {"border", 247},
};

/*
DRY: единственная точка, где имя превращается в ANSI-строку.
*/
string getColor(const string &name)
{
    int number = colorNumbers[name];
    if (number < 0)
        return "  ";
    return ansi(number);
}

struct Figure
{
    string name;
    string color;
    vector<vector<int>> shape;
};

/*
DRY: одна таблица фигур вместо switch с семью кейсами
и копипастой map[..] = 2 в каждом.
*/
vector<Figure> figures = {
    {"T", "violet", {{0, 1, 0}, {1, 1, 1}}},
    {"S", "green", {{0, 1, 1}, {1, 1, 0}}},
    {"Z", "red", {{1, 1, 0}, {0, 1, 1}}},
    {"L", "orange", {{1, 1, 1}, {1, 0, 0}}},
    {"J", "blue", {{1, 0, 0}, {1, 1, 1}}},
    {"I", "cyan", {{1, 1, 1, 1}}},
    {"O", "yellow", {{1, 1}, {1, 1}}},
};

class Map
{
public:
    bool inBounds(int y, int x) const
    {
        return y >= 0 && y < totalHeight() && x >= 0 && x < width;
    }

    bool isEmpty(int y, int x) const
    {
        return !isFixed(y, x);
    }

    void set(int y, int x, const string &color)
    {
        map[y][x] = color;
    }

    void clearCell(int y, int x)
    {
        map[y][x] = (y < hidden) ? "hidden" : "empty";
    }

    int clearFullLayers()
    {
        int cleared = 0;
        vector<vector<string>> newMap = makeMap(width, height, hidden);
        int dstRow = hidden + height - 1;

        for (int y = 0; y < hidden; y++)
            for (int x = 0; x < width; x++)
                newMap[y][x] = map[y][x];

        for (int y = 0; y < height; y++)
        {
            int srcRow = hidden + height - 1 - y;
            bool full = true;
            for (int x = 0; x < width; x++)
            {
                if (!isFixed(srcRow, x))
                {
                    full = false;
                    break;
                }
            }
            if (full)
            {
                cleared++;
            }
            else
            {
                newMap[dstRow] = map[srcRow];
                dstRow--;
            }
        }

        map = newMap;
        return cleared;
    }

    // KISS: цикл с y = hidden, скрытые строки не печатаются.
    void render() const
    {
        string border = getColor("border");

        cout << endl;
        for (int i = 0; i < width + 2; i++)
            cout << border;
        cout << endl;

        for (int y = hidden; y < totalHeight(); y++)
        {
            cout << border;
            for (int x = 0; x < width; x++)
                cout << getColor(map[y][x]);
            cout << border;
            cout << endl;
        }

        for (int i = 0; i < width + 2; i++)
            cout << border;
        cout << endl;
    }

private:
    int totalHeight() const { return hidden + height; }

    int width = 5;
    int height = 7;
    int hidden = 2;

    // KISS: понятный предикат вместо map[y][x] >= 3 по всему коду.
    bool isFixed(int y, int x) const
    {
        string cell = map[y][x];
        return cell != "hidden" && cell != "empty" && cell != "active";
    }

    /*
    DRY: карта строится в одном месте.
    */
    static vector<vector<string>> makeMap(int width, int height, int hidden)
    {
        vector<vector<string>> map(hidden + height, vector<string>(width, "empty"));
        for (int y = 0; y < hidden; y++)
            for (int x = 0; x < width; x++)
                map[y][x] = "hidden";
        return map;
    }

    vector<vector<string>> map = makeMap(width, height, hidden);
};

/*
KISS: класс разделён на два, Map (поле) и GameRules (логика игры).
*/
class GameRules
{
public:
    GameRules(Map &b) : board(b) {}

    bool isFigureActive() const { return hasFigure; }
    bool isAlive() const { return alive; }
    int getScore() const { return score; }
    string getFigureName() const { return figureName; }

    void render() const { board.render(); }

    // KISS: Map возвращает число линий, очки, это правила игры, ему место здесь.
    void clearLines() { score += board.clearFullLayers() * 10; }

    void make_figure()
    {
        Figure figure = figures[nextFigure];

        nextFigure = nextFigure + 1;
        if (nextFigure >= (int)figures.size())
            nextFigure = 0;

        hasFigure = true;
        figureX = 0;
        figureY = 0;
        color = figure.color;
        figureName = figure.name;

        shape = figure.shape;
        shapeHeight = shape.size();
        shapeWidth = shape[0].size();

        drawShape();
    }

    // DRY: right/left, это обёртки над move(dx, dy), а не три похожих метода.
    void right() { move(1, 0); }
    void left() { move(-1, 0); }

    void fall()
    {
        if (!move(0, 1))
        {
            if (figureY > 1)
                lock_figure();
            else
                alive = false;
        }
    }

    // DRY: turn_right/turn_left, это обёртки над turn(direction).
    void turn_right() { turn(Clockwise); }
    void turn_left() { turn(CounterClockwise); }

    void fall_down()
    {
        while (hasFigure == true && alive == true)
        {
            fall();
        }
    }

private:
    Map &board;

    int score = 0;
    bool alive = true;
    bool hasFigure = false;
    int nextFigure = 0;

    int shapeWidth = 0;
    int shapeHeight = 0;
    int figureX = 0;
    int figureY = 0;
    vector<vector<int>> shape;
    string color = "violet";
    string figureName = "";

    // DRY: двойной цикл рисования в одном месте.
    void drawShape()
    {
        for (int y = 0; y < shapeHeight; y++)
            for (int x = 0; x < shapeWidth; x++)
                if (shape[y][x] == 1)
                    board.set(figureY + y, figureX + x, "active");
    }

    // DRY: двойной цикл стирания в одном месте.
    void eraseShape()
    {
        for (int y = 0; y < shapeHeight; y++)
            for (int x = 0; x < shapeWidth; x++)
                if (shape[y][x] == 1)
                    board.clearCell(figureY + y, figureX + x);
    }

    // KISS: чистая проверка, ничего не меняет.
    bool canMove(int dx, int dy) const
    {
        for (int y = 0; y < shapeHeight; y++)
            for (int x = 0; x < shapeWidth; x++)
                if (shape[y][x] == 1)
                {
                    int boardY = figureY + dy + y;
                    int boardX = figureX + dx + x;
                    if (!board.inBounds(boardY, boardX))
                        return false;
                    if (!board.isEmpty(boardY, boardX))
                        return false;
                }
        return true;
    }

    // DRY: один move на все направления.
    bool move(int dx, int dy)
    {
        if (!canMove(dx, dy))
            return false;

        eraseShape();
        figureX = figureX + dx;
        figureY = figureY + dy;
        drawShape();
        return true;
    }

    bool canTurn(TurnDirection direction) const
    {
        for (int y = 0; y < shapeHeight; y++)
            for (int x = 0; x < shapeWidth; x++)
                if (shape[y][x] == 1)
                {
                    int boardY, boardX;
                    if (direction == Clockwise)
                    {
                        boardY = figureY + x;
                        boardX = figureX + shapeHeight - y - 1;
                    }
                    else
                    {
                        boardY = figureY + shapeWidth - x - 1;
                        boardX = figureX + y;
                    }
                    if (!board.inBounds(boardY, boardX))
                        return false;
                    if (!board.isEmpty(boardY, boardX))
                        return false;
                }
        return true;
    }

    // DRY: один turn на оба направления.
    void turn(TurnDirection direction)
    {
        if (!canTurn(direction))
            return;

        eraseShape();

        vector<vector<int>> newShape(shapeWidth, vector<int>(shapeHeight));
        for (int y = 0; y < shapeHeight; y++)
            for (int x = 0; x < shapeWidth; x++)
            {
                if (direction == Clockwise)
                    newShape[x][shapeHeight - y - 1] = shape[y][x];
                else
                    newShape[shapeWidth - x - 1][y] = shape[y][x];
            }

        int temp = shapeWidth;
        shapeWidth = shapeHeight;
        shapeHeight = temp;

        shape = newShape;

        drawShape();
    }

    void lock_figure()
    {
        for (int y = 0; y < shapeHeight; y++)
            for (int x = 0; x < shapeWidth; x++)
                if (shape[y][x] == 1)
                    board.set(figureY + y, figureX + x, color);
        hasFigure = false;
        score++;
    }
};

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    system("chcp 65001 > nul");

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif

    Map gameMap;
    GameRules game(gameMap);

    int steps = 10;
    int timer = 8;
    char move;

    cout << "A - влево, S - Вниз на 1 слой, D - Вправо, W - В самый низ" << endl;
    cout << "Введите что-нибудь, чтобы продолжить" << endl;
    cin >> move;

    while (game.isAlive() == true)
    {
        cout << "Очков набранно " << game.getScore() << endl;
        cout << "Фигура: " << game.getFigureName() << endl;
        cout << "Шагов осталось " << steps << endl;
        cout << "Меньше шагов через " << timer << endl;

        if (game.isFigureActive() == false)
        {
            game.make_figure();
            steps = 10;
            timer = 8;
        }
        else
        {
            if (timer <= 0)
            {
                timer = 8;
                if (steps > 1)
                    steps--;
            }
            if (steps <= 0)
            {
                game.fall();
                steps = 10;
            }
            else
            {
                steps--;
                cin >> move;
                switch (move)
                {
                case 'd':
                    game.right();
                    break;
                case 'a':
                    game.left();
                    break;
                case 'q':
                    game.turn_left();
                    break;
                case 'e':
                    game.turn_right();
                    break;
                case 's':
                {
                    game.fall();
                    if (timer == 0 && steps > 1)
                    {
                        steps--;
                        timer = 8;
                    }
                    else
                    {
                        timer--;
                    }
                    break;
                }
                case 'w':
                {
                    timer--;
                    game.fall_down();
                    break;
                }
                }
            }
        }

        game.clearLines();

#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        game.render();
    }

    cout << "ВСЁ" << endl;
    cout << "Вы набрали " << game.getScore() << " очков" << endl;
}