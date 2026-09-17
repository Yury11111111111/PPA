#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <queue>
#include <algorithm>
using namespace std;

void check(string& in) { //ПРОВЕРКА ВХОДЯЩИХ ДАННЫХ
	bool flag;
	do {
    	cout << "Введите число: ";
    	getline(cin, in);
    	flag = false;
    	if (in.size() > 4 || in.size() < 4 || in[0] == '0') {
        	cout << "Число должно состоять из 4-х цифр и не начинаться с 0\n";
        	flag = true;
        	continue;
    	}
    	for (auto& sym : in) {
        	if (!isdigit(sym)) {
            	cout << "Число должно состоять из 4-х цифр\n";
            	flag = true;
            	break;
        	}
    	}
    	if (flag) continue;
    	for (int i = 0; i < 4; i++) {
        	for (int j = i + 1; j < 4; j++) {
            	if (in[i] == in[j]) {
                	flag = true;
                	break;
            	}
        	}
        	if (flag) {
            	cout << "Число должно состоять из 4-х неповторяющихся цифр\n";
            	break;
        	}
    	}
	} while (flag);
}

int set(const string& in, char c, int index) {
	if (in[index] == c) {
    	return 2; //Бык
	}
	for (const auto& el : in) {
    	if (el == c)
        	return 1; //Корова
	}
	return 0; //Ничего
}

int main()
{
    int qwe;
    cout<<"Начало работы программы"<<endl;
	setlocale(LC_ALL, "ru");
	string in;
	check(in);
	queue<char> digits({ '1','2','3','4','5','6','7','8','9','0'});
	map <char, std::set<int>> cows;
	vector<char>bulls(4);
	string select;
	do {
	    select = "";
    	for (int i = 0; i < 4; i++) {
        	bool flag = false;
        	if (digits.empty() && not(bulls[i]==0)) {
            	select.push_back(bulls[i]);
            	continue;
        	}
        	for (auto& el : cows) {
            	if ((count(el.second.begin(), el.second.end(), i) == 0) && (count(select.begin(), select.end(), el.first) == 0)) {
                	select.push_back(el.first);
                	el.second.insert(i);
                	flag = true;
                	break;
            	}
        	}
        	if (flag) continue;
        	if (digits.empty() ) {
            	if(bulls[i] != 0)
                	select.push_back(bulls[i]);
            	else {
                	for (int j = 0; j < 10; j++) {
                    	if ((count(bulls.begin(), bulls.end(), j+48) == 0) && (count_if(cows.begin(), cows.end(), [&](const pair<char, std::set<int>>& el) {return char(j+48) == el.first; }) == 0)) {
                        	select.push_back(bulls[j]);
                        	break;
                    	}
                	}
            	}
        	}
        	else {
            	select.push_back(digits.front());
            	digits.pop();
        	}
    	}
    	cout <<select<< endl;
    	for (int i = 0; i < 4; i++) {
        	int res = ::set(in, select[i], i);
        	switch (res) {
        	case 2:{
            	cout << "Б";
            	bulls[i] = select[i];
            	for (auto& el : cows) {
                	el.second.insert(i);
                	if (el.second.size() == 3) {
                    	for (int j = 0; j < 4; j++) {
                        	if (count(el.second.begin(), el.second.end(), j) == 0) {
                            	bulls[j] = el.first;
                        	}
                    	}
                	}
            	}
            	{
                	auto it = find_if(cows.begin(), cows.end(), [&](const pair<char, std::set<int>>& p) {return select[i] == p.first; });
                	if (it != cows.end())
                    	cows.erase(it);
            	}
            	
            	break;
        	}
        	case 1:{
            	cout << "К";
            	cows[select[i]].insert(i);
            	for (int j = 0; j < 4; j++) {
                	if (bulls[j] != 0) {
                    	cows[select[i]].insert(j);
                	}
            	}

                if (cows[select[i]].size() == 3) {
                    for (int j = 0; j < 4; j++) {
                        if (count(cows[select[i]].begin(), cows[select[i]].end(), j) == 0) {
                            bulls[j] = select[i];
                            for (auto& el : cows) {
                                el.second.insert(j);
                            }
                            {
                                auto it = find_if(cows.begin(), cows.end(), [&](const pair<char, std::set<int>>& p) {return select[i] == p.first; });
                                if (it != cows.end())
                                    cows.erase(it);
                            }
                        }
                    }
                }
                break;
        	}
            default:{
                cout << "Н";
            }
            }
        }
        cout << endl;
    } while (count(bulls.begin(), bulls.end(), 0) != 0);

    for (const auto& el : bulls) {
        cout << el;
    }
    cout << endl << "ББББ"<<endl;
    cout<<"Конец работы программы";
}
