#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "LCG.h"

using namespace std;

void make_data(int seed = 10){ // n - количество колок
    int n = 9000; // Все кол-во запусков
    int k = 3; // Кол-во отрезков для проверки
    LCG lcg(seed);
    int t_p, t_obr;
    vector<vector<int>> t_p_count(k, vector<int> (6, 0));
    vector<vector<int>> t_obr_count(k, vector<int> (6, 0));
    for(int k1=1; k1<k+1; k1++){
        for(int i=n/k*(k1-1)+1; i<n/k*k1; i++){
            t_p = lcg.generate_t_p();
            t_obr =lcg.generate_t_obr();
            t_p_count[k1-1][t_p-1]++;
            t_obr_count[k1-1][t_obr-1]++;
        }
    }
    int pre;
    std::ofstream fout;          // поток для записи
    fout.open("test_distribution_data.txt");      // открываем файл для записи
    fout << n << " " << k << endl;
    fout << "t_p" <<endl;
    for(int k1 = 0; k1 < k; k1++){
        fout << "k: " << k1 << endl;
        pre = 0;
        for(int i = 0; i< 6; i++){
                pre += t_p_count[k1][i]*100/n;
                fout << i+1 << ": " << t_p_count[k1][i] << " " << t_p_count[k1][i]*100/n << " " << pre << endl;
        }
    }
    cout << "t_k done" << endl;
    fout << "t_obr" <<endl;
    for(int k1=0; k1<k; k1++){
        fout << "k: " << k1 << endl;
        pre = 0;
        for(int i = 0; i< 6; i++){
            pre += t_obr_count[k1][i]*100/n;
            fout << i+1 << ": " << t_obr_count[k1][i] << " " << t_obr_count[k1][i]*100/n << " " << pre << pre*100/n << endl;        }
    }
    cout << "t_obr done" << endl;
    fout.close();
}
int main(int argc, char** argv){
	if (argc == 1){
		make_data();
		return 0;
	}
	if (argc == 2){
		make_data(atoi(argv[0]));
		return 0;
	}
	cout << "Передано слишком много аргументов. Ожидается один." <<endl;
}
