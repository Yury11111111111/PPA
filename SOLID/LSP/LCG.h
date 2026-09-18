#include <iostream>
#include <random>
#include <fstream>
#include <list>

using namespace std;


random_device rd;
mt19937 gen(rd());

struct LCG {
    int state;
    LCG(){}
    void addSeed(int _seed){
	state = _seed;
	srand(_seed);
    }
    LCG(int _seed){ 
        state = _seed;
        srand(_seed);
    }
    int generate_100(){
        int r = rand();
        if(r < 0){ r *= -1; }
        r = r%101;
        return r;
    }
    int generate_t_p(){
        int r = generate_100();
        if(r < 25){
            return 1;
        }
        if(r < 48){
            return 2;
        }
        if(r < 69){
            return 3;
        }
        if(r < 81){
            return 4;
        }
        if(r < 90){
            return 5;
        }
        return 6;
    }
    int generate_t_obr(){
        int r = generate_100();
        if(r < 6){
            return 1;
        }
        if(r < 21){
            return 2;
        }
        if(r < 48){
            return 3;
        }
        if(r < 77){
            return 4;
        }
        if(r < 83){
            return 5;
        }
        return 6;
    }
};
