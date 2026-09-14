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

    template <typename Action>
    void for_each_figure_cell(
        const Figure& figure,
        int y_coords,
        int x_coords,
        Action action
    ) {
        for (int y = 0; y < figure.model.size(); ++y) {
            for (int x = 0; x < figure.model[y].size(); ++x) {
                if (figure.model[y][x]) {
                    action(y_coords + y, x_coords + x);
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
        for (int y = 0; y < buffer_layers; ++y) {
            cleared_map[y] = map[y];
        }
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

    static bool is_fixed(Cell cell) {
        return cell >= Cell::FigureT && cell <= Cell::FigureO;
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
    void draw_figure(const Figure& figure, int y_coords, int x_coords, Cell cell) {
        for_each_figure_cell(figure, y_coords, x_coords, [this, cell](int y, int x) {
            map[y][x] = cell;
        });
    }

    void erase_figure(const Figure& figure, int y_coords, int x_coords) {
        for_each_figure_cell(figure, y_coords, x_coords, [this](int y, int x) {
            map[y][x] = y < buffer_layers ? Cell::Hidden : Cell::Empty;
        });
    }

    // Возвращаем, живы ли еще
    bool fix_figure(const Figure& figure, int x_coords, int y_coords) {
        if (y_coords < buffer_layers) {
            return false;
        }
        draw_figure(figure, y_coords, x_coords, figure.color);
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
            map.erase_figure(figure, y_coords, x_coords);
            figure.model = rotated;
            map.draw_figure(figure, y_coords, x_coords, Cell::Active);
        }
    }

    bool try_move(int dx, int dy) {
        if (!map.can_place(figure.model, x_coords + dx, y_coords + dy)) {
            return false;
        }
        map.erase_figure(figure, y_coords, x_coords);
        x_coords += dx;
        y_coords += dy;
        map.draw_figure(figure, y_coords, x_coords, Cell::Active);
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
        map.draw_figure(figure, y_coords, x_coords, Cell::Active);
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
        points += map.clear_full_layers() * points_per_layer;
        system("clear");
        map.print_map();
    }
};

/*
KISS: Вынесла таймер в отдельный класс, а то иначе понять что происходит было трудно
*/
/*
YAGNI: Возможность задавать пармаметры конфигурации не нужна, значения всегда берутся по умолчанию
*/
class StepsCounter {
    int upper_bound_timer = 5; // Раз в сколько кол-во шагов уменьшается
    int upper_bound_steps = 5; //Сколько шагов по горизонтали можно сделать

    int current_timer = 15; //Текущий таймер для игры
    int current_steps = 15; //Сколько шагов по горизонтали можно сделать сейчас
public:
    void tick() { current_timer--; }

    int get_timer() { return current_timer; }
    int get_steps() { return current_steps; }

    void reset_steps() {
        current_steps = upper_bound_steps;
    }

    // Возвращаем, произошло ли принудительное падение
    bool make_step(){
        if(current_timer <= 0){
            upper_bound_steps = max(1, upper_bound_steps - 1);
            current_timer = upper_bound_timer;
            current_steps = min(current_steps, upper_bound_steps);
        }
        tick();
        if(current_steps <= 0){
            current_steps = upper_bound_steps;
            return true;
        }
        current_steps--;
        return false;
    }
};

int main()
{
    Engine engine;

    StepsCounter stepCounter;

    char move;
    cout << "A - влево, S - Вниз на 1 слой, D - Вправо, W - В самый низ" << endl;
    cout << "Введите что-нибудь, чтобы продолжить"<<endl;
    if (!(cin >> move)) { return 0; }
    while(engine.get_alive() == 1){
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
        else{
            if(stepCounter.make_step()){
                engine.fall();
            }
            else{
                if (!(cin >> move)) { return 0; }
                switch(move){
                    case 'd':{
                        engine.move(1);
                        break;
                    }
                    case 'a':{
                        engine.move(-1);
                        break;
                    }
                    case 'q':{
                        engine.turn_left();
                        break;
                    }
                    case 'e':{
                        engine.turn_right();
                        break;
                    }
                    case 's':{
                        /*
                        KISS: Раньше расчеты с уменьшением кол-ва шагов были очень сложно написаны,
                        еле можно было разобрать что к чему
                        Теперь они были вынесены в отдельный класс, и понять что происходит проще
                        */
                        engine.fall();
                        break;
                    }
                    case 'w':{
                        engine.fall_down();
                    }
                }
                
            }
        }
        engine.end_iteration();
    }
    cout << "ВСЁ" << endl;
    cout << "Вы набрали " << engine.get_points() << " очков" << endl;
    return 0;
}
