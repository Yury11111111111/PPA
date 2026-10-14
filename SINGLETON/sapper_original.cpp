#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>

const int SIZE = 9;
const int MINES = 10;

struct Cell {
    bool mine = false;
    bool revealed = false;
    bool flagged = false;
    int adjacent = 0;
};

class Minesweeper {
public:
    Minesweeper() { reset(); }

    void reset() {
        for (int r = 0; r < SIZE; r++)
            for (int c = 0; c < SIZE; c++)
                board[r][c] = Cell{};
        revealedCount = 0;
        flagCount = 0;
        correctFlags = 0;
        gameOver = false;
        win = false;
        placeMines();
        computeAdjacent();
    }

    void reveal(int r, int c) {
        if (gameOver) return;
        if (!inBounds(r, c)) return;
        if (board[r][c].flagged) return;
        revealFlood(r, c);
        checkWin();
    }

    void toggleFlag(int r, int c) {
        if (gameOver) return;
        if (!inBounds(r, c)) return;
        Cell& cell = board[r][c];
        if (cell.revealed) return;

        cell.flagged = !cell.flagged;
        int delta = cell.flagged ? 1 : -1;
        flagCount += delta;
        if (cell.mine) correctFlags += delta;

        checkWin();
    }

    bool isOver() const { return gameOver; }
    bool isWin()  const { return win; }
    int  flagsLeft() const { return MINES - flagCount; }

    void print() const {
        std::cout << "    ";
        for (int c = 0; c < SIZE; c++) std::cout << c << ' ';
        std::cout << "\n   ";
        for (int c = 0; c < SIZE; c++) std::cout << "--";
        std::cout << "\n";

        for (int r = 0; r < SIZE; r++) {
            std::cout << r << " | ";
            for (int c = 0; c < SIZE; c++) {
                const Cell& cell = board[r][c];
                if (gameOver && cell.mine && !cell.flagged) {
                    std::cout << "* ";
                } else if (cell.flagged) {
                    std::cout << "F ";
                } else if (!cell.revealed) {
                    std::cout << "# ";
                } else if (cell.adjacent == 0) {
                    std::cout << ". ";
                } else {
                    std::cout << cell.adjacent << ' ';
                }
            }
            std::cout << "\n";
        }
        std::cout << "Флагов осталось: " << flagsLeft() << "\n";
    }

private:
    Cell board[SIZE][SIZE];
    int revealedCount = 0;
    int flagCount = 0;     
    int correctFlags = 0; 
    bool gameOver = false;
    bool win = false;

    static bool inBounds(int r, int c) {
        return r >= 0 && r < SIZE && c >= 0 && c < SIZE;
    }

    void checkWin() {
        if (gameOver) return;
        bool allSafeRevealed = (revealedCount == SIZE * SIZE - MINES);
        bool allMinesFlagged = (correctFlags == MINES && flagCount == MINES);
        if (allSafeRevealed || allMinesFlagged) {
            win = true;
            gameOver = true;
        }
    }

    void placeMines() {
        int placed = 0;
        while (placed < MINES) {
            int r = std::rand() % SIZE;
            int c = std::rand() % SIZE;
            if (!board[r][c].mine) {
                board[r][c].mine = true;
                placed++;
            }
        }
    }

    void computeAdjacent() {
        for (int r = 0; r < SIZE; r++) {
            for (int c = 0; c < SIZE; c++) {
                if (board[r][c].mine) continue;
                int cnt = 0;
                for (int dr = -1; dr <= 1; dr++)
                    for (int dc = -1; dc <= 1; dc++) {
                        if (dr == 0 && dc == 0) continue;
                        int nr = r + dr, nc = c + dc;
                        if (inBounds(nr, nc) && board[nr][nc].mine) cnt++;
                    }
                board[r][c].adjacent = cnt;
            }
        }
    }

    void revealFlood(int r, int c) {
        if (!inBounds(r, c)) return;
        Cell& cell = board[r][c];
        if (cell.revealed || cell.flagged) return;

        cell.revealed = true;

        if (cell.mine) {
            gameOver = true;
            return;
        }
        revealedCount++; 

        if (cell.adjacent == 0) {
            for (int dr = -1; dr <= 1; dr++)
                for (int dc = -1; dc <= 1; dc++)
                    if (dr || dc) revealFlood(r + dr, c + dc);
        }
    }
};

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    Minesweeper game;

    std::cout << "=== САПЕР ===\n";
    std::cout << "Команды: r <строка> <столбец> — открыть, "
                 "f <строка> <столбец> — флаг\n\n";

    while (!game.isOver()) {
        game.print();
        std::cout << "> ";
        char cmd;
        int r, c;
        if (!(std::cin >> cmd)) return 0;
        std::cin >> r >> c;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Неверный ввод.\n";
            continue;
        }

        if (cmd == 'r' || cmd == 'R') {
            game.reveal(r, c);
        } else if (cmd == 'f' || cmd == 'F') {
            game.toggleFlag(r, c);
        } else {
            std::cout << "Неизвестная команда\n";
        }
    }

    game.print();
    if (game.isWin()) {
        std::cout << "\nBig Win! Все мины найдены\n";
    } else {
        std::cout << "\nПупупу! Вы наступили на мину(\n";
    }
    return 0;
}