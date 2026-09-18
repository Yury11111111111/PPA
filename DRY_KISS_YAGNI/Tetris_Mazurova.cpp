#include <iostream>
#include <vector>
#include <array>
#include <cstdio>

using namespace std;

// KISS: именованные константы вместо магических чисел 0..9
constexpr int MAP_W = 5;
constexpr int MAP_H = 9;          
constexpr int SPAWN_ROWS = 2;
constexpr int EMPTY        = 0;   
constexpr int BLANK        = 1;   
constexpr int ACTIVE       = 2;   
constexpr int LOCKED_FIRST = 3;   

// DRY: массив цветов вместо десяти case в render()
const char* CELL_COLOR[] = {
    "\033[48:5:238m  \033[m", // 0 EMPTY
    "  ",                     // 1 BLANK
    "\033[48:5:255m  \033[m", // 2 ACTIVE
    "\033[48:5:105m  \033[m", // 3 T
    "\033[48:5:10m  \033[m",  // 4 S
    "\033[48:5:160m  \033[m", // 5 Z
    "\033[48:5:208m  \033[m", // 6 L
    "\033[48:5:75m  \033[m",  // 7 J
    "\033[48:5:123m  \033[m", // 8 I
    "\033[48:5:220m  \033[m", // 9 O
};
const char* FRAME = "\033[48:5:247m  \033[m";

// DRY: фигуры как данные, а не как семь case
struct Shape {
    int w, h;
    vector<vector<int>> cells;
};

const vector<Shape> SHAPES = {
    {3, 2, {{0,1,0},{1,1,1}}}, // T
    {3, 2, {{0,1,1},{1,1,0}}}, // S
    {3, 2, {{1,1,0},{0,1,1}}}, // Z
    {3, 2, {{1,1,1},{1,0,0}}}, // L
    {3, 2, {{1,0,0},{1,1,1}}}, // J
    {4, 1, {{1,1,1,1}}},       // I
    {2, 2, {{1,1},{1,1}}},     // O
};

class Map {
public:
    bool figure_exists() const { return figure_exists_; }
    bool alive()         const { return alive_; }
    int  points()        const { return points_; }

//  KISS: активная фигура накладывается при рендере,а не хранится в map
    void render() const {
        cout << endl;
        for (int i = 0; i < MAP_W + 2; ++i) printf("%s", FRAME);
        cout << endl;
        for (int y = 0; y < MAP_H; ++y) {
            printf("%s", FRAME);
            for (int x = 0; x < MAP_W; ++x) {
                int cell = map_[y][x];
                if (figure_exists_ &&
                    y >= y_ && y < y_ + model_h_ &&
                    x >= x_ && x < x_ + model_w_ &&
                    model_[y - y_][x - x_] == 1) {
                    cell = ACTIVE;
                }
                printf("%s", CELL_COLOR[cell]);
            }
            printf("%s", FRAME);
            cout << endl;
        }
        for (int i = 0; i < MAP_W + 2; ++i) printf("%s", FRAME);
        cout << endl;
    }

    // DRY: фигуры и цвета берутся из таблицы, а не из switch
    void make_figure() {
        figure_exists_ = true;
        x_ = 0;
        y_ = 0;
        color_ = step_ + LOCKED_FIRST;
        const Shape& s = SHAPES[step_];
        model_   = s.cells;
        model_w_ = s.w;
        model_h_ = s.h;
        step_ = (step_ + 1) % static_cast<int>(SHAPES.size());
    }

// DRY, одна общая проверка столкновений для всех движений
    bool can_move(int dx, int dy) const {
        if (x_ + dx < 0) return false;
        if (x_ + dx + model_w_ > MAP_W) return false;
        if (y_ + dy + model_h_ > MAP_H) return false;
        for (int y = 0; y < model_h_; ++y)
            for (int x = 0; x < model_w_; ++x)
                if (model_[y][x] == 1 &&
                    map_[y_ + dy + y][x_ + dx + x] >= LOCKED_FIRST)
                    return false;
        return true;
    }

    void move(int dx, int dy) {
        if (can_move(dx, dy)) { x_ += dx; y_ += dy; }
    }

    // DRY right/left — просто обёртки над move
    void right() { move( 1, 0); }
    void left()  { move(-1, 0); }

    void fall() {
        if (can_move(0, 1)) { y_ += 1; return; }
        if (y_ > 1) lock();
        else alive_ = false;
    }

    void fall_down() {
        while (figure_exists_ && alive_) fall();
    }

    void lock() {
        for (int y = 0; y < model_h_; ++y)
            for (int x = 0; x < model_w_; ++x)
                if (model_[y][x] == 1)
                    map_[y_ + y][x_ + x] = color_;
        figure_exists_ = false;
        points_ += 1;
    }

    // KISS: без VLA (int ret[x][y] — нестандарт), без ручного транспонирования с индексами
    static vector<vector<int>> rotate_cw(const vector<vector<int>>& m, int h, int w) {
        vector<vector<int>> r(w, vector<int>(h));
        for (int y = 0; y < w; ++y)
            for (int x = 0; x < h; ++x)
                r[y][x] = m[h - 1 - x][y];
        return r;
    }

    static vector<vector<int>> rotate_ccw(const vector<vector<int>>& m, int h, int w) {
        vector<vector<int>> r(w, vector<int>(h));
        for (int y = 0; y < w; ++y)
            for (int x = 0; x < h; ++x)
                r[y][x] = m[x][w - 1 - y];
        return r;
    }

    // DRY: одна проверка и одна установка для обоих поворотов
    void try_rotate(vector<vector<int>> new_model, int new_h, int new_w) {
        if (x_ + new_w > MAP_W) return;
        if (y_ + new_h > MAP_H) return;
        for (int y = 0; y < new_h; ++y)
            for (int x = 0; x < new_w; ++x)
                if (new_model[y][x] == 1 &&
                    map_[y_ + y][x_ + x] >= LOCKED_FIRST)
                    return;
        model_   = std::move(new_model);
        model_w_ = new_w;
        model_h_ = new_h;
    }

    void turn_right() {
        try_rotate(rotate_cw (model_, model_h_, model_w_), model_w_, model_h_);
    }
    void turn_left() {
        try_rotate(rotate_ccw(model_, model_h_, model_w_), model_w_, model_h_);
    }

    // DRY: цикл один, без захардкоженного ret-массива
    // KISS: ясно, что происходит — снизу вверх сдвигаем неполные ряды, полные на  удаление
    void clear_full_layers() {
        decltype(map_) new_map = {};
        for (int y = 0; y < MAP_H; ++y)
            new_map[y].fill(BLANK);
        new_map[0] = map_[0];                 
        new_map[1] = map_[1];

        int write_y = MAP_H - 1;
        for (int y = MAP_H - 1; y >= SPAWN_ROWS; --y) {
            bool full = true;
            for (int x = 0; x < MAP_W; ++x)
                if (map_[y][x] < LOCKED_FIRST) { full = false; break; }
            if (full) points_ += 10;
            else { new_map[write_y] = map_[y]; --write_y; }
        }
        map_ = new_map;
    }

private:
    int points_         = 0;
    bool alive_         = true;
    bool figure_exists_ = false;

    // YAGNI фикс размеры —- std::array вместо vector<vector>
    array<array<int, MAP_W>, MAP_H> map_ = {{
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {BLANK, BLANK, BLANK, BLANK, BLANK},
        {BLANK, BLANK, BLANK, BLANK, BLANK},
        {BLANK, BLANK, BLANK, BLANK, BLANK},
        {BLANK, BLANK, BLANK, BLANK, BLANK},
        {BLANK, BLANK, BLANK, BLANK, BLANK},
        {BLANK, BLANK, BLANK, BLANK, BLANK},
        {BLANK, BLANK, BLANK, BLANK, BLANK}
    }};

    int x_ = 0, y_ = 0;
    int model_w_ = 0, model_h_ = 0;
    vector<vector<int>> model_;
    int color_ = 0;
    int step_  = 0;
};

int main() {
    Map m;
    int const_steps = 10;
    int const_timer = 8;
    int timer = const_timer;
    int steps = const_steps;
    char move;

    // KISS -- +добавлены отсутствовавшие Q/E в подсказку
    cout << "A - влево, S - вниз на 1, D - вправо, W - в самый низ" << endl;
    cout << "Q - поворот влево, E - поворот вправо" << endl;
    cout << "Введите что-нибудь, чтобы продолжить" << endl;
    cin >> move;

    while (m.alive()) {
        cout << "Очков набрано " << m.points() << endl;
        cout << "Шагов осталось " << steps << endl;
        cout << "Меньше шагов через " << timer << endl;

        if (!m.figure_exists()) {
            m.make_figure();
            steps = const_steps;
        } else {
            if (timer <= 0) { timer = const_timer; --const_steps; }
            if (steps <= 0) {
                m.fall();
                steps = const_steps;
            } else {
                --steps;
                cin >> move;
                switch (move) {
                    case 'd': m.right();      break;
                    case 'a': m.left();       break;
                    case 'q': m.turn_left();  break;
                    case 'e': m.turn_right(); break;
                    case 's': {
                        m.fall();
                        // DRY, одна логика таймера на все случаи
                        if (timer == 0 && const_steps > 1) {
                            --const_steps;
                            timer = const_timer;
                            steps = const_steps;
                        } else {
                            --timer;
                            steps = const_steps;
                        }
                        break;
                    }
                    case 'w': {
                        --timer;
                        m.fall_down();
                        break;
                    }
                }
            }
        }
        m.clear_full_layers();
        // KISS/portability (ANSI вместо system("clear")
        cout << "\033[2J\033[H";
        m.render();
    }
    cout << "ВСЁ" << endl;
    cout << "Вы набрали " << m.points() << " очков" << endl;
}
