#include <iostream>
#include <deque>
#include <string>
#include "LCG_Liskov.h"

using namespace std;

long long check_copy(LCG& lcg, int check_size){
        lcg.addSeed(10);

        deque<int> check_order;
        for(int i=0; i<check_size; i++){
                check_order.push_back(lcg.generate_time());
        }

        deque<int> gen_order = check_order;
        for(long long i = 1; ; i++){
                gen_order.pop_front();
                gen_order.push_back(lcg.generate_time());

                if( check_order == gen_order){
                        return i;
                }
        }
}

void test_50(LCG& lcg, const string& name){
        const int launches = 1000;
        int event_count = 0;

        lcg.addSeed(10);

        for(int i = 0; i < launches; i++){
                if(lcg.generate_100() >= 50){
                        event_count++;
                }
        }

        cout << name << ": событие произошло " << event_count << " раз из " << launches << endl;
}

int main(){
        LCG lcg(10);
        LCG_t_p lcg_t_p(10);
        LCG_t_obr lcg_t_obr(10);

        test_50(lcg, "LCG");
        test_50(lcg_t_p, "LCG_t_p");
        test_50(lcg_t_obr, "LCG_t_obr");

        cout << endl;

        cout << "LCG: " << check_copy(lcg, 3) << endl;
        cout << "LCG_t_p: " << check_copy(lcg_t_p, 6) << endl;
        cout << "LCG_t_obr: " << check_copy(lcg_t_obr, 6) << endl;
}
