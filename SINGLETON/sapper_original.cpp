#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

const int SIZE = 9;
const int MINES = 10;

struct Cell {
    bool mine = false;
    bool revealed = false;
    bool flagged = false;
    int adjacent = 0;
};

class Minesweeper {
    std::vector<std::vector<Cell>> board;
    int revealedCount = 0;
    bool gameOver = false;
    bool win = false;

public:
    Minesweeper() : board(SIZE, std::vector<Cell>(SIZE)) {
        placeMines();
        computeAdjacent();
    }

    void placeMines() {
        std::srand(std::time(nullptr));
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
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        if (dr == 0 && dc == 0) continue;
                        int nr = r + dr, nc = c + dc;
                        if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE &&
                            board[nr][nc].mine) cnt++;
                    }
                }
                board[r][c].adjacent = cnt;
            }
        }
    }

    void revealFlood(int r, int c) {
        if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) return;
        Cell& cell = board[r][c];
        if (cell.revealed || cell.flagged) return;

        cell.revealed = true;
        revealedCount++;

        if (cell.mine) {
            gameOver = true;
            return;
        }

        if (cell.adjacent == 0) {
            for (int dr = -1; dr <= 1; dr++)
                for (int dc = -1; dc <= 1; dc++)
                    if (dr || dc) revealFlood(r + dr, c + dc);
        }
    }

    void reveal(int r, int c) {
        if (gameOver) return;
        if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) return;
        if (board[r][c].flagged) return;
        revealFlood(r, c);

        if (!gameOver && revealedCount == SIZE * SIZE - MINES) {
            win = true;
            gameOver = true;
        }
    }

    void toggleFlag(int r, int c) {
        if (gameOver) return;
        if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) return;
        if (board[r][c].revealed) return;
        board[r][c].flagged = !board[r][c].flagged;
    }

    bool isOver() const { return gameOver; }
    bool isWin() const { return win; }

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
    }
};

int main() {
    Minesweeper game;

    std::cout << "=== САПЕР ===\n";
    std::cout << "Команды: r <строка> <столбец> — открыть, "
                 "f <строка> <столбец> — флаг\n\n";

    while (!game.isOver()) {
        game.print();
        std::cout << "> ";
        char cmd;
        int r, c;
        std::cin >> cmd >> r >> c;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Неверный ввод.\n";
            continue;
        }

        if (cmd == 'r' || cmd == 'R') {
            game.reveal(r, c);
        } else if (cmd == 'f' || cmd == 'F') {
            game.toggleFlag(r, c);
        } else {
            std::cout << "Неизвестная команда.\n";
        }
    }

    game.print();
    if (game.isWin()) {
        std::cout << "\n🎉 Победа! Все мины найдены.\n";
    } else {
        std::cout << "\n💥 Бум! Вы наступили на мину.\n";
    }
    return 0;
}
