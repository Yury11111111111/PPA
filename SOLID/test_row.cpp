#include <iostream>
#include "LCG.h"

int check_copy(bool t_func = 0, int seed = 10){
        LCG lcg(seed);
        int check_size = 10;
        string check_100;
        string gen_100 = "";
        int gen_last;
        for(int i=0; i<check_size; i++){
                if(t_func){
                        gen_last = lcg.generate_t_p();
                }
                else{
                        gen_last = lcg.generate_t_obr();
                }
                gen_100 += to_string(gen_last);
        }
        check_100 = gen_100;
        for(int i = 0; ; i++){
                if(t_func){
                        gen_last = lcg.generate_t_p();
                }
                else{
                        gen_last = lcg.generate_t_obr();
                }
                gen_100.erase(0, 1);
                gen_100 += to_string(gen_last);
                if( check_100 == gen_100){
                        return i;
                }
        }
        return -1;
}

int main(){ // n - количество колок
        cout << "Раз в сколько запусков повторяется последовательность из 10 чисел для:"<<endl;
        cout << "t_k: " << check_copy(0) << endl << "t_obr: " << check_copy(1) <<endl;
}
