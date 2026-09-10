/*
printf("\033[48:5:160m  \033[m\n"); - красный Z
printf("\033[48:5:208m  \033[m\n"); - оранжевый L
printf("\033[48:5:220m  \033[m\n"); - желтый O
printf("\033[48:5:75m  \033[m\n"); - синий J
printf("\033[48:5:105m  \033[m\n"); - фиолетовый T
printf("\033[48:5:123m  \033[m\n"); - голубой I
printf("\033[48:5:10m  \033[m\n"); - зеленый S
printf("\033[48:5:255m  \033[m\n"); - белый
printf("\033[48:5:247m  \033[m"); - серый
printf("\033[48:5:238m  \033[m"); - темно-серый
*/

#include <iostream>
#include <vector>
#include <stdio.h>

using namespace std;

class Map{
    public:
        bool get_figure_exist(){return figure_exist;}
        bool get_alive(){return alive;}
        void render(){
            cout<<endl;
            for(int i = 0; i< x_map+2; i++){
                printf("\033[48:5:247m  \033[m");
            }
            cout<<endl;
            for(int y = 0; y<y_map+2; y++){
                printf("\033[48:5:247m  \033[m");
                for(int x = 0; x<x_map; x++){
                    switch(map[y][x]){
                        case 0:{
                            printf("\033[48:5:238m  \033[m");
                            break;
                        }
                        case 1:{
                            cout<<"  ";
                            break;
                        }
                        case 2:{
                            printf("\033[48:5:255m  \033[m");
                            break;
                        }
                        case 3:{
                            printf("\033[48:5:105m  \033[m");
                            break;
                        }
                        case 4:{
                            printf("\033[48:5:10m  \033[m");
                            break;
                        }
                        case 5:{
                            printf("\033[48:5:160m  \033[m");
                            break;
                        }
                        case 6:{
                            printf("\033[48:5:208m  \033[m");
                            break;
                        }
                        case 7:{
                            printf("\033[48:5:75m  \033[m");
                            break;
                        }
                        case 8:{
                            printf("\033[48:5:123m  \033[m");
                            break;
                        }
                        case 9:{
                            printf("\033[48:5:220m  \033[m");
                            break;
                        }
                    }
                }
                printf("\033[48:5:247m  \033[m");
                cout<<endl;
            }
            for(int i = 0; i< x_map+2; i++){
                printf("\033[48:5:247m  \033[m");
            }
            cout<<endl;
        }
        void make_figure(){
            figure_exist = 1;
            x_coords = 0;
            y_coords = 0;
            color = step+3;
            switch(step){
                case 0: //T
                    step++;
                    x_model = 3;
                    y_model = 2;
                    model = {{0,1,0},
                             {1,1,1}};
                    map[0][1] = 2;
                    map[1][0] = 2;
                    map[1][1] = 2;
                    map[1][2] = 2;
                    break;
                case 1: //S
                    step++;
                    x_model = 3;
                    y_model = 2;
                    model = {{0,1,1},
                             {1,1,0}};
                    map[0][1] = 2;
                    map[0][2] = 2;
                    map[1][0] = 2;
                    map[1][1] = 2;
                    break;
                case 2: //Z
                    step++;
                    x_model = 3;
                    y_model = 2;
                    model = {{1,1,0},
                             {0,1,1}};
                    map[0][0] = 2;
                    map[0][1] = 2;
                    map[1][1] = 2;
                    map[1][2] = 2;
                    break;
                case 3: //L
                    step++;
                    x_model = 3;
                    y_model = 2;
                    model = {{1,1,1},
                             {1,0,0}};
                    map[0][0] = 2;
                    map[0][1] = 2;
                    map[0][2] = 2;
                    map[1][0] = 2;
                    break;
                case 4: //J
                    step++;
                    x_model = 3;
                    y_model = 2;
                    model = {{1,0,0},
                             {1,1,1}};
                    map[0][0] = 2;
                    map[1][0] = 2;
                    map[1][1] = 2;
                    map[1][2] = 2;
                    break;
                case 5: //I
                    step++;
                    x_model = 4;
                    y_model = 1;
                    model = {{1,1,1,1}};
                    map[0][0] = 2;
                    map[0][1] = 2;
                    map[0][2] = 2;
                    map[0][3] = 2;
                    break;
                case 6: //O
                    step=0;
                    x_model = 2;
                    y_model = 2;
                    model = {{1,1},
                             {1,1}};
                    map[0][0] = 2;
                    map[0][1] = 2;
                    map[1][0] = 2;
                    map[1][1] = 2;
                    break;
            }
        }
        void right(){
            if(x_coords+x_model<x_map){
                bool k = 1;
                for(int y = 0; y<y_model; y++){
                    for(int x = 0; x<x_model; x++){
                        if(model[y][x] == 1 and map[y_coords+y][x_coords+x+1] >= 3){
                            k = 0;
                            break;
                        }
                    }
                }
                if(k==1){
                    for(int y = 0; y<y_model; y++){
                        for(int x = 0; x<x_model; x++){
                            if(model[y_model-y-1][x_model-x-1] == 1){
                                cout<<5<<endl;
                                if(y_coords+y_model-y-1<2){
                                    map[y_coords+y_model-y-1][x_coords+x_model-x-1] = 0;
                                }
                                else{map[y_coords+y_model-y-1][x_coords+x_model-x-1] = 1;}
                                map[y_coords+y_model-y-1][x_coords+x_model-x] = 2;
                            }
                        }
                    }
                    x_coords++;
                }
            }
        }
        void left(){
            if(x_coords>0){
                bool k = 1;
                for(int y = 0; y<y_model; y++){
                    for(int x = 0; x<x_model; x++){
                        if(model[y][x] == 1 and map[y_coords+y][x_coords+x-1] >= 3){
                            k = 0;
                            break;
                        }
                    }
                }
                if(k==1){
                    for(int y = 0; y<y_model; y++){
                        for(int x = 0; x<x_model; x++){
                            if(model[y][x] == 1){
                                if(y_coords+y<2){
                                    map[y_coords+y][x_coords+x] = 0;
                                }
                                else{map[y_coords+y][x_coords+x] = 1;}
                                map[y_coords+y][x_coords+x-1] = 2;
                            }
                        }
                    }
                    x_coords--;
                }
            }
        }
        void fall(){
            if(y_coords+y_model<y_map+2){
                bool k = 1;
                for(int y = 0; y<y_model; y++){
                    for(int x = 0; x<x_model; x++){
                        if(model[y][x] == 1 and map[y_coords+y+1][x_coords+x] >= 3){
                            k = 0;
                            break;
                        }
                    }
                }
                if(k==1){
                    for(int y = 0; y<y_model; y++){
                        for(int x = 0; x<x_model; x++){
                            if(model[y_model-y-1][x] == 1){
                                if(y_coords+y_model-y-1<2){
                                    map[y_coords+y_model-y-1][x_coords+x] = 0;
                                }
                                else{map[y_coords+y_model-y-1][x_coords+x] = 1;}
                                map[y_coords+y_model-y][x_coords+x] = 2;
                            }
                        }
                    }
                    y_coords++;
                }
                else{
                    if(y_coords>1){
                        for(int y = 0; y<y_model; y++){
                            for(int x = 0; x<x_model; x++){
                                if(model[y][x] == 1){
                                    map[y_coords+y][x_coords+x] = color;
                                }
                            }
                        }
                        figure_exist = 0;
                        points++;
                    }
                    else{
                        alive = 0;
                    }
                }
            }
            else{
                if(y_coords>1){
                    for(int y = 0; y<y_model; y++){
                        for(int x = 0; x<x_model; x++){
                            if(model[y][x] == 1){
                                map[y_coords+y][x_coords+x] = color;
                            }
                        }
                    }
                    figure_exist = 0;
                    points++;
                }
                else{
                    alive = 0;
                }
            }
        }
        void clear_full_layers(){
            bool k;
            vector<vector<int>> ret = {{0,0,0,0,0},
                                       {0,0,0,0,0},
                                       {1,1,1,1,1},
                                       {1,1,1,1,1},
                                       {1,1,1,1,1},
                                       {1,1,1,1,1},
                                       {1,1,1,1,1},
                                       {1,1,1,1,1},
                                       {1,1,1,1,1}};
            int i_y = y_map+1;
            for(int y = 0; y<2; y++){
                for(int x = 0; x<x_map; x++){
                    ret[y][x] = map[y][x];
                }
            }
            for(int y = 0; y<y_map; y++){
                k = 1;
                for(int x = 0; x<x_map; x++){
                    if(map[y_map-y+1][x]<3){
                        k = 0;
                        break;
                    }
                }
                if(k==1){
                    points+=10;
                }
                else{
                    ret[i_y] = map[y_map-y+1];
                    i_y--; 
                }
            }
            
            map = ret;
        }
        int get_points(){return points;}
        void turn_right(){
            if(x_coords+y_model<=x_map){
                bool k = 1;
                int ret [x_model][y_model];
                for(int y = 0; y<y_model; y++){
                    for(int x = 0; x<x_model; x++){
                        if(model[y][x] == 1 and map[y_coords+x][x_coords+y_model-y-1]>2){
                            k = 0;
                            break;
                        }
                        else{
                            ret[x][y_model-y-1] = model[y][x];
                        }
                    }
                }
                if(k==1){
                    for(int y = 0; y<y_model; y++){
                        for(int x = 0; x<x_model;x++){
                            if(y_coords+y<2){
                                map[y_coords+y][x_coords+x] = 0;
                            }
                            else{
                                map[y_coords+y][x_coords+x] = 1;
                            }
                        }
                    }
                    int delta = x_model;
                    x_model = y_model;
                    y_model = delta;
                    model.resize(y_model);
                    for(int y = 0; y<y_model; y++){
                        model[y].resize(x_model);
                        for(int x = 0; x<x_model; x++){
                            model[y][x] = ret[y][x];
                            if(model[y][x] == 1){
                                map[y_coords+y][x_coords+x] = 2;
                            }
                        }
                    }
                }
            }
        }
        void turn_left(){
            bool k = 1;
            int ret [x_model][y_model];
            for(int y = 0; y<y_model; y++){
                for(int x = 0; x<x_model; x++){
                    if(model[y][x] == 1 and map[y_coords+x_model-x-1][x_coords+y]>2){
                        k = 0;
                        break;
                    }
                    else{
                        ret[x_model-x-1][y] = model[y][x];
                    }
                }
            }
            if(k==1){
                for(int y = 0; y<y_model; y++){
                    for(int x = 0; x<x_model;x++){
                        if(y_coords+y<2){
                            map[y_coords+y][x_coords+x] = 0;
                        }
                        else{
                            map[y_coords+y][x_coords+x] = 1;
                        }
                    }
                }
                int delta = x_model;
                x_model = y_model;
                y_model = delta;
                model.resize(y_model);
                for(int y = 0; y<y_model; y++){
                    model[y].resize(x_model);
                    for(int x = 0; x<x_model; x++){
                        model[y][x] = ret[y][x];
                        if(model[y][x] == 1){
                            map[y_coords+y][x_coords+x] = 2;
                        }
                    }
                }
            }
        }
        void fall_down(){
            while(figure_exist == 1 and alive == 1){
                fall();
            }
        }
    private:
    int points = 0;
    bool alive = 1;
    bool figure_exist = 0;
    int x_map = 5;
    int y_map = 7;
    vector<vector<int>> map = {{0,0,0,0,0},
                               {0,0,0,0,0},
                               {1,1,1,1,1},
                               {1,1,1,1,1},
                               {1,1,1,1,1},
                               {1,1,1,1,1},
                               {1,1,1,1,1},
                               {1,1,1,1,1},
                               {1,1,1,1,1}};
    int x_model;
    int y_model;
    int x_coords;
    int y_coords;
    vector<vector<int>> model;
    int color;
    
    int step = 0;
};

int main()
{
    Map m;
    int const_steps = 10; //Сколько шагов по горизонтали можно сделать сейчас
    int const_timer = 8;
    int timer = const_timer; //Раз в сколько кол-во шагов уменьшается
    int steps = const_steps; //Сколько шагов по горизонтали можно сделать
    char move;
    cout<<"A - влево, S - Вниз на 1 слой, D - Вправо, W - В самый низ"<<endl;
    cout<<"Введите что-нибудь, чтобы продолжить"<<endl;
    cin>>move;
    while(m.get_alive() == 1){
        cout<<"Очков набранно "<<m.get_points()<<endl;
        cout<<"Шагов осталось "<<steps<<endl;
        cout<<"Меньше шагов через "<<timer<<endl;
        if(m.get_figure_exist() == 0){
            m.make_figure();
            steps = const_steps;
        }
        else{
            if(timer <= 0){
                timer=const_timer;
                const_steps--;
            }
            if(steps <= 0){
                m.fall();
                steps = const_steps;
            }
            else{
                steps--;
                cin>>move;
                switch(move){
                    case 'd':{
                        m.right();
                        break;
                    }
                    case 'a':{
                        m.left();
                        break;
                    }
                    case 'q':{
                        m.turn_left();
                        break;
                    }
                    case 'e':{
                        m.turn_right();
                        break;
                    }
                    case 's':{
                        m.fall();
                        if(timer == 0 and const_steps > 1){
                            const_steps--;
                            timer = const_timer;
                            steps = const_steps;
                        }
                        else{
                            timer--;
                            steps = const_steps;
                        }
                        break;
                    }
                    case 'w':{
                        timer--;
                        m.fall_down();
                    }
                }
                
            }
        }
        m.clear_full_layers();
        system("clear");
        m.render();
    }
    cout<<"ВСЁ"<<endl;
    cout<<"Вы набрали "<<m.get_points()<<" очков"<<endl;
}