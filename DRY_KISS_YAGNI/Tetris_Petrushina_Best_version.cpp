/*
Улучшенная версия с исправлениями по DRY, KISS, YAGNI
Автор: Петрушина Юлия
*/

#include <iostream>
#include <vector>
#include <algorithm> // all_of, min, max
#include <cstdlib> // system("clear")
#include <map>
#include <string>

using namespace std;

/*
KISS: обозначаю что в клетке не через рандомные цифры, а как понятные элементы из enum
*/
enum Cell {
    Hidden = 0,
    Empty = 1,
    Active = 2,
    FigureT = 3,
    FigureS = 4,
    FigureZ = 5,
    FigureL = 6,
    FigureJ = 7,
    FigureI = 8,
    FigureO = 9,
    Border = 10
};

const map<Cell, string> cell_color = {
    {Cell::Hidden, "238"},
    {Cell::Active, "255"},
    {Cell::FigureT, "105"},
    {Cell::FigureS, "10"},
    {Cell::FigureZ, "160"},
    {Cell::FigureL, "208"},
    {Cell::FigureJ, "75"},
    {Cell::FigureI, "123"},
    {Cell::FigureO, "220"},
    {Cell::Border, "247"}
};

/*
DRY: Вместо огромного switch для отрисовки клетки нужного цвета,
использую словарь с получением цвета по элементу Cell:: enum.
В эту функцию просто передается тип клетки, и она ее отрисовывает
*/
void print_cell(const Cell& cell) {
    if (cell == Cell::Empty) {
        cout << "  ";
    } else {
        cout << "\033[48:5:" + cell_color.at(cell) + "m  \033[m";
    }
}

struct Figure {
    vector<vector<int>> model;
    Cell color;
};

/*
DRY: Вместо огромного switch с заполнением данных о фигурах, использую один вектор с перечеслением всех фигур.
Нужную можно получить по индексу, а работа с ними единообразна
*/
const vector<Figure> FIGURES = {
    {{{0, 1, 0},
      {1, 1, 1}}, FigureT},

    {{{0, 1, 1},
      {1, 1, 0}}, FigureS},

    {{{1, 1, 0},
      {0, 1, 1}}, FigureZ},

    {{{1, 1, 1},
      {1, 0, 0}}, FigureL},

    {{{1, 0, 0},
      {1, 1, 1}}, FigureJ},

    {{{1, 1, 1, 1}}, FigureI},

    {{{1, 1},
      {1, 1}}, FigureO}
};

/*
YAGNI: убрала из конструкторов классов все лишние переменные
Для маштабируемости было бы хорошо задавание кол-ва слоев и колонок, 
но бы принимаем, что текущая версия - финальная
И тогда такое задавание не имеет смысла, ведь всегда берется значение по умолчанию
*/
class Map {
private:
    static constexpr int x_map = 5;
    static constexpr int y_map = 7;
    static constexpr int buffer_layers = 2;

    vector<vector<Cell>> map;

    /*
    KISS: В разных функциях был разный порядок x и y
    Из-за этого было трудно понимать логику, постоянно путалась как правильно в функцию передавать
    Я сделала везде всегда сначала x потом y
    */
    template <typename Action>
    void for_each_figure_cell(
        const Figure& figure,
        int x_coords,
        int y_coords,
        Action action
    ) {
        for (size_t y = 0; y < figure.model.size(); ++y) {
            for (size_t x = 0; x < figure.model[y].size(); ++x) {
                if (figure.model[y][x]) {
                    action(
                        x_coords + static_cast<int>(x),
                        y_coords + static_cast<int>(y)
                    );
                }
            }
        }
    }

    /*
    YAGNI: Нам не нужна функция для построения чистой карты с задаваемыми параметрами, 
    нам нужна функция для создания пустой карты с константными параметрами 5 x 7
    */
    /*
    YAGNI: Изначально функция была public, но это лишнее
    Достаточно чтобы она была private
    */
    void make_empty_map(vector<vector<Cell>>& some_map){
        some_map.resize(y_map+buffer_layers);
        for(int i = 0; i<y_map+buffer_layers; i++){
            some_map[i] = vector<Cell>(x_map, (i < buffer_layers) ? Cell::Hidden : Cell::Empty);
        }
    }

    /*
    YAGNI: Эта функция вызывается только внутри класса, она не нужна как public
    */
    static bool is_fixed(Cell cell) {
        return cell >= Cell::FigureT && cell <= Cell::FigureO;
    }

public:
    Map()
    {
        make_empty_map(map);
    }

    void print_map() {
        cout << endl;
        for (int x = 0; x < x_map + 2; ++x) {
            print_cell(Cell::Border);
        }
        cout << endl;
        for (const auto& row : map) {
            print_cell(Cell::Border);
            for (Cell cell : row) {
                print_cell(cell);
            }
            print_cell(Cell::Border);
            cout << endl;
        }
        for (int x = 0; x < x_map + 2; ++x) {
            print_cell(Cell::Border);
        }
        cout << endl;
    }

    int clear_full_layers() {
        vector<vector<Cell>> cleared_map;
        make_empty_map(cleared_map);
        int target_y = y_map + buffer_layers - 1;
        int count_layers = 0;
        for (int y = target_y; y >= buffer_layers; --y) {
            /*
            KISS: Весто того, чтобы писать свою функцию на один раз, используем из стандартной библиотеки
            */
            const bool full = all_of(map[y].begin(), map[y].end(), is_fixed);
            if (full) {
                count_layers++;
            } else {
                cleared_map[target_y--] = map[y];
            }
        }
        map = cleared_map;
        return count_layers;
    }

    bool can_place(const vector<vector<int>>& candidate, int left, int top) const {
        const int height = candidate.size();
        const int width = candidate[0].size();
        if (left < 0 || top < 0 || left + width > x_map ||
            top + height > y_map + buffer_layers) {
            return false;
        }
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (candidate[y][x] && is_fixed(map[top + y][left + x])) {
                    return false;
                }
            }
        }
        return true;
    }

    /*
    DRY: Зачем повторять обход всех клеток и в draw_figure и erase_figure?
    Вынесем его в отдельную функцию for_each_figure_cell
    Туда будем просто передавать, что делать с клеткой
    */
    void draw_figure(const Figure& figure, int x_coords, int y_coords, Cell cell) {
        for_each_figure_cell(figure, x_coords, y_coords, [this, cell](int x, int y) {
            map[y][x] = cell;
        });
    }

    void erase_figure(const Figure& figure, int x_coords, int y_coords) {
        for_each_figure_cell(figure, x_coords, y_coords, [this](int x, int y) {
            map[y][x] = y < buffer_layers ? Cell::Hidden : Cell::Empty;
        });
    }

    // Возвращаем, живы ли еще
    bool fix_figure(const Figure& figure, int x_coords, int y_coords) {
        if (y_coords < buffer_layers) {
            return false;
        }
        draw_figure(figure, x_coords, y_coords, figure.color);
        return true;
    }
};

/*
Для следования KISS я разделила один большой и тяжелый для понимания класс Map на два:
- Map отвечает за саму карту
- Engine отвечает за логику игры
*/
class Engine {
    int points_per_figure = 1;
    int points_per_layer = 10;

    int points = 0;
    bool alive = true;
    bool figure_exist = false;
    int x_coords = 0;
    int y_coords = 0;

    Map map;
    Figure figure;

    size_t step = 0;

    void turn(bool clockwise) {
        if (!figure_exist || !alive) {
            return;
        }

        const int height = figure.model.size();
        const int width = figure.model[0].size();
        vector<vector<int>> rotated(width, vector<int>(height));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (clockwise) {
                    rotated[x][height - y - 1] = figure.model[y][x];
                } else {
                    rotated[width - x - 1][y] = figure.model[y][x];
                }
            }
        }
        // Если поворот невозможен, оставляем фигуру в том же положении что и до этого, иначе:
        if (map.can_place(rotated, x_coords, y_coords)) {
            map.erase_figure(figure, x_coords, y_coords);
            figure.model = rotated;
            map.draw_figure(figure, x_coords, y_coords, Cell::Active);
        }
    }

    bool try_move(int dx, int dy) {
        if (!map.can_place(figure.model, x_coords + dx, y_coords + dy)) {
            return false;
        }
        map.erase_figure(figure, x_coords, y_coords);
        x_coords += dx;
        y_coords += dy;
        map.draw_figure(figure, x_coords, y_coords, Cell::Active);
        return true;
    }

public:
    bool get_figure_exist() { return figure_exist; }
    bool get_alive() { return alive; }
    int get_points() { return points; }

    void make_figure() {
        if (figure_exist || !alive) {
            return;
        }
        figure = FIGURES[step];
        x_coords = 0;
        y_coords = 0;
        if (!map.can_place(figure.model, x_coords, y_coords)) {
            alive = false;
            return;
        }
        step = (step + 1) % FIGURES.size();
        figure_exist = true;
        map.draw_figure(figure, x_coords, y_coords, Cell::Active);
    }

    // direction: влево = -1, вправо = 1
    void move(int direction) {
        if (direction != -1 && direction != 1) {
            cout << "Invalid direction!" << endl;
            exit(1);
        }
        if (figure_exist && alive) {
            try_move(direction, 0);
        }
    }

    void turn_right() { turn(true); }
    void turn_left() { turn(false); }

    // Это упасть вниз на 1 слой
    void fall() {
        if (!figure_exist || !alive) {
            return;
        }
        if (!try_move(0, 1)) {
            if (map.fix_figure(figure, x_coords, y_coords)) {
                figure_exist = false;
                points += points_per_figure;
                points += map.clear_full_layers() * points_per_layer;
            } else {
                alive = false;
            }

        }
    }

    // Это упасть вниз с концами
    void fall_down() {
        while (figure_exist && alive) {
            fall();
        }
    }

    void end_iteration() {
        system("clear");
        map.print_map();
    }
};

/*
KISS: Вынесла обработку команд в отдельную функцию
Это дало меньшую вложенность, а значит понимать проще
*/
void process_command(Engine& engine, char command) {
    switch (command) {
        case 'd':
            engine.move(1);
            break;
        case 'a':
            engine.move(-1);
            break;
        case 'q':
            engine.turn_left();
            break;
        case 'e':
            engine.turn_right();
            break;
        case 's':
            engine.fall();
            break;
        case 'w':
            engine.fall_down();
            break;
    }
}

/*
KISS: Вынесла таймер в отдельный класс, а то иначе понять что происходит было трудно
*/
/*
YAGNI: Возможность задавать пармаметры конфигурации не нужна, значения всегда берутся по умолчанию
*/
/*
KISS: Улучшила реализацию таймера, все что можно убрать внутрь убрала
*/
class StepsCounter {
    static constexpr int difficulty_interval = 15;
    static constexpr int initial_step_limit = 5;

    int iterations_until_difficulty_increase = difficulty_interval;
    int step_limit = initial_step_limit;
    int remaining_steps = step_limit;

    void update_difficulty() {
        iterations_until_difficulty_increase--;
        if (iterations_until_difficulty_increase > 0) {
            return;
        }

        step_limit = max(1, step_limit - 1);
        remaining_steps = min(remaining_steps, step_limit);
        iterations_until_difficulty_increase = difficulty_interval;
    }

public:
    int get_timer() const { return iterations_until_difficulty_increase; }
    int get_steps() const { return remaining_steps; }

    void reset_steps() {
        remaining_steps = step_limit;
    }

    bool should_force_fall() {
        update_difficulty();

        if (remaining_steps == 0) {
            reset_steps();
            return true;
        }

        remaining_steps--;
        return false;
    }
};

int main()
{
    Engine engine;
    StepsCounter stepCounter;

    char move;
    cout << "A - влево, S - Вниз на 1 слой, D - Вправо, W - В самый низ" << endl;
    cout << "Q - повернуть фигуру влево, E - повернуть фигуру вправо" << endl;
    cout << "Введите что-нибудь, чтобы продолжить"<<endl;
    if (!(cin >> move)) { return 0; }
    /*
    KISS: Зачем проверять true, через == 1, если можно проверить через !
    */
    while(engine.get_alive()){
        cout<<"Очков набранно " << engine.get_points() << endl;
        cout<<"Шагов осталось " << stepCounter.get_steps() << endl;
        cout<<"Меньше шагов через " << stepCounter.get_timer() << endl;
        /*
        KISS: Зачем проверять false, через == 0, если можно проверить через !
        */
        if(!engine.get_figure_exist()){
            engine.make_figure();
            stepCounter.reset_steps();
        }
        else if(stepCounter.should_force_fall()){
                engine.fall();
        }
        else{
            if (!(cin >> move)) { return 0; }
            process_command(engine, move);
        }
        engine.end_iteration();
    }
    cout << "ВСЁ" << endl;
    cout << "Вы набрали " << engine.get_points() << " очков" << endl;
    return 0;
}
