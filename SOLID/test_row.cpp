#include <iostream>
#include "LCG_Liskov.h"

int check_copy(LCG& lcg){
        int check_size = 10;
        string check_100;
        string gen_100 = "";
        int gen_last;
        for(int i=0; i<check_size; i++){
                gen_last = lcg.generate();
                gen_100 += to_string(gen_last);
        }
        check_100 = gen_100;
        for(int i = 0; ; i++){
                gen_last = lcg.generate();
                gen_100.erase(0, 1);
                gen_100 += to_string(gen_last);
                if( check_100 == gen_100){
                        return i;
                }
        }
        return -1;
}

int main(){ // n - количество колок
        LCG_t_p lcg_t_p(10); // время между машинами
        LCG_t_obr lcg_t_obr(10); // время заправки машины

        cout << "Раз в сколько запусков повторяется последовательность из 10 чисел для:"<<endl;
        cout << "t_k: " << check_copy(lcg_t_p) << endl << "t_obr: " << check_copy(lcg_t_obr) <<endl;
}
