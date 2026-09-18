#include <iostream> 
#include <list>
#include <cmath>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <cassert>
using namespace std;

class Optimated_Divisors{
    public:
        int is_number_simple(int number){
            check_number_in_divisors(number);
            return( data_divisors.at(number).size() == 2); 
        }
        int is_number_perfect(int number){
            check_number_in_divisors(number);
            return (data_summs.at(number) == 2 * number);
        }
        int get_greatest_common_divisor(int number1, int number2){
            check_numbers_in_GCD(number1, number2);
            return data_GCD.at({number1, number2});
        }
        int get_least_common_multiple(int number1, int number2){
            check_numbers_in_GCD(number1, number2);
            return number1 * number2 / data_GCD.at({number1, number2});
        }
    private:
        map <int, list <int>> data_divisors;
        map <int, int> data_summs;
        bool is_key_in_data_divisors(int number){
            return data_divisors.count(number)>0;
        }
        void add_divisors_and_summ(int number){
            list<int> divisors;
            int summa = 0;
            for (int i=1; i< number/2+1; i++)
            {
                if(number%i == 0){
                    divisors.push_back(i);
                    summa += i;
                }
            }
            divisors.push_back (number);
            summa += number;
            data_divisors.insert(pair<int, list<int>> {number, divisors});
            data_summs.insert(pair<int, int> {number, summa});
        }
        void check_number_in_divisors(int number){
            if(not(is_key_in_data_divisors(number))){
                add_divisors_and_summ(number);
            }
        }
        map <pair<int, int>, int> data_GCD;
        bool is_key_in_data_GCD(pair<int, int> numbers){
            return data_GCD.count(numbers)>0;
        }
        void add_GCD(int number1, int number2){
            check_number_in_divisors(number1);
            check_number_in_divisors(number2);
            list<int>::iterator i1 = data_divisors.at(number1).begin();
            list<int>::iterator i2 = data_divisors.at(number2).begin();
            int max_common_divisor = 1;
            while(i1!=data_divisors.at(number1).end(), i2!=data_divisors.at(number2).end()){
                if(*i1==*i2){
                    max_common_divisor = *i1;
                    i1++;
                    i2++;
                }
                else if(*i1<*i2){
                    if(i1==data_divisors.at(number1).end()){
                        break;
                    }
                    i1++;
                }
                else{
                    if(i2==data_divisors.at(number2).end()){
                        break;
                    }
                    i2++;
                }
            }
            data_GCD.insert(pair <pair<int, int>, int> {{number1, number2}, max_common_divisor});
        }
        void check_numbers_in_GCD(int number1, int number2){
            if(not(is_key_in_data_GCD({number1, number2}))){
                add_GCD(number1, number2);
            }
        }
};

class Formalization_data{
    public:
    string work (string input){
        if(count(input.begin(), input.end(), ' ')==1){
            try{
                int number1 = stoi(input.substr(0, input.find(' ')));
                int number2 = stoi(input.substr(input.find(' ')+1, input.length()-input.find(' ')));

                bool number1_simple = space.is_number_simple(number1);
                bool number1_perfect = space.is_number_perfect(number1);
                bool number2_simple = space.is_number_simple(number2);
                bool number2_perfect = space.is_number_perfect(number2);
                int GCD = space.get_greatest_common_divisor(number1, number2);
                int LCM = space.get_least_common_multiple(number1, number2);

                return form_response_GCD_and_LCM(input.substr(0, input.find(' ')), input.substr(input.find(' ')+1, input.length()-input.find(' ')), number1_simple, number1_perfect, number2_simple, number2_perfect, GCD, LCM);
            }
            catch (...) {
                return "Error";
            }
        }
        else if(count(input.begin(), input.end(), '+')==1){
            try{
                pair <pair<int, int>, pair<int, int>> numbers = split_fractions(input, '+');

                int ret_up = (numbers.first.first * space.get_least_common_multiple(numbers.first.second, numbers.second.second) /  numbers.first.second) + (numbers.second.first * space.get_least_common_multiple(numbers.first.second, numbers.second.second) /  numbers.second.second);
                int ret_down = space.get_least_common_multiple(numbers.first.second, numbers.second.second);

                pair <int, int> ret = reduce_fraction(ret_up, ret_down);
                
                return form_response_fractions(ret.first, ret.second);
            }
            catch (...) {
                return "Error";
            }
        }
        else if(count(input.begin(), input.end(), '-')==1){
            try{
                pair <pair<int, int>, pair<int, int>> numbers = split_fractions(input, '-');

                int ret_up = (numbers.first.first * space.get_least_common_multiple(numbers.first.second, numbers.second.second) /  numbers.first.second) - (numbers.second.first * space.get_least_common_multiple(numbers.first.second, numbers.second.second) /  numbers.second.second);
                int ret_down = space.get_least_common_multiple(numbers.first.second, numbers.second.second);

                pair <int, int> ret = reduce_fraction(ret_up, ret_down);

                return form_response_fractions(ret.first, ret.second);
            }
            catch (...) {
                return "Error";
            }
        }
        else{
            return "Error";
        }
    }
    private:
    pair <pair<int, int>, pair<int, int>> split_fractions (string input, char operation){
        return { {
                    stoi(input.substr(0, input.find('/'))), 
                    stoi(input.substr(input.find('/')+1, input.find(operation)-input.find('/')-1))
                }, 
                { 
                    stoi(input.substr(input.find(operation)+1, input.find('/', 2)-input.find(operation)-1)), 
                    stoi(input.substr(input.find('/', input.find('/')+1)+1, input.length()-input.find('/', input.find('/')+1)-1))
                }};
    }
    pair <int, int> reduce_fraction(int ret_up, int ret_down){
        int reduction = space.get_greatest_common_divisor(abs(ret_up), ret_down);
        if (reduction != 1){
            ret_up = ret_up/reduction;
            ret_down = ret_down/reduction;
        }
        return {ret_up, ret_down};
    }
    string form_response_GCD_and_LCM(string number1, string number2, bool number1_simple, bool number1_perfect, bool number2_simple, bool number2_perfect, int GCD, int LCM){
        return number1 + string (number1_simple ? " простое и " : " не простое и ") + string (number1_perfect ? "совершенное, " : "не совершенное, ") + 
                number2 + (number2_simple ? " простое и" : " не простое и") + (number2_perfect ? " совершенное, " : " не совершенное, ") + "НОД = " 
                + to_string(GCD) + ", НОК = " + to_string(LCM);
    }
    string form_response_fractions(int ret_up, int ret_down){
        if(ret_down == 1 or ret_up == 0){
            return "Ответ: " + to_string(ret_up);
        }
        return "Ответ: " + to_string(ret_up) + '/' +  to_string(ret_down);
    }
    Optimated_Divisors space;
};

const vector <pair<string, string>> tested_sets_GCD_and_LCM = {{"2 128", "2 простое и не совершенное, 128 не простое и не совершенное, НОД = 2, НОК = 128"},
                                                            {"128 2", "128 не простое и не совершенное, 2 простое и не совершенное, НОД = 2, НОК = 128"},
                                                            {"16 4", "16 не простое и не совершенное, 4 не простое и не совершенное, НОД = 4, НОК = 16"},
                                                            {"4 16", "4 не простое и не совершенное, 16 не простое и не совершенное, НОД = 4, НОК = 16"},
                                                            {"9 3", "9 не простое и не совершенное, 3 простое и не совершенное, НОД = 3, НОК = 9"},
                                                            {"3 9", "3 простое и не совершенное, 9 не простое и не совершенное, НОД = 3, НОК = 9"}, 
                                                            {"4 7", "4 не простое и не совершенное, 7 простое и не совершенное, НОД = 1, НОК = 28"},
                                                            {"7 4", "7 простое и не совершенное, 4 не простое и не совершенное, НОД = 1, НОК = 28"},
                                                            {"31 5", "31 простое и не совершенное, 5 простое и не совершенное, НОД = 1, НОК = 155"},
                                                            {"5 31", "5 простое и не совершенное, 31 простое и не совершенное, НОД = 1, НОК = 155"},
                                                            {"6 5", "6 не простое и совершенное, 5 простое и не совершенное, НОД = 1, НОК = 30"},
                                                            {"5 6", "5 простое и не совершенное, 6 не простое и совершенное, НОД = 1, НОК = 30"},
                                                            {"16 28", "16 не простое и не совершенное, 28 не простое и совершенное, НОД = 4, НОК = 112"},
                                                            {"28 16", "28 не простое и совершенное, 16 не простое и не совершенное, НОД = 4, НОК = 112"},
                                                            {"18 42", "18 не простое и не совершенное, 42 не простое и не совершенное, НОД = 6, НОК = 126"},
                                                            {"42 18", "42 не простое и не совершенное, 18 не простое и не совершенное, НОД = 6, НОК = 126"},
                                                            {"8 60", "8 не простое и не совершенное, 60 не простое и не совершенное, НОД = 4, НОК = 120"},
                                                            {"60 8", "60 не простое и не совершенное, 8 не простое и не совершенное, НОД = 4, НОК = 120"},
                                                            {"1 2", "1 не простое и не совершенное, 2 простое и не совершенное, НОД = 1, НОК = 2"},
                                                            {"2 1", "2 простое и не совершенное, 1 не простое и не совершенное, НОД = 1, НОК = 2"},
                                                            {"2 3", "2 простое и не совершенное, 3 простое и не совершенное, НОД = 1, НОК = 6"},
                                                            {"3 2", "3 простое и не совершенное, 2 простое и не совершенное, НОД = 1, НОК = 6"},
                                                            {"3 1", "3 простое и не совершенное, 1 не простое и не совершенное, НОД = 1, НОК = 3"},
                                                            {"1 3", "1 не простое и не совершенное, 3 простое и не совершенное, НОД = 1, НОК = 3"},
                                                            {"15 25", "15 не простое и не совершенное, 25 не простое и не совершенное, НОД = 5, НОК = 75"},
                                                            {"25 15", "25 не простое и не совершенное, 15 не простое и не совершенное, НОД = 5, НОК = 75"}
};
const vector <pair<string, string>> tested_sets_fractions = {{"1/2+3/4", "Ответ: 5/4"},
                                                        {"3/4+1/2", "Ответ: 5/4"},
                                                        {"1/2+1/2", "Ответ: 1"},
                                                        {"1/2+2/4", "Ответ: 1"},
                                                        {"2/4+1/2", "Ответ: 1"},
                                                        {"1/2+3/6", "Ответ: 1"},
                                                        {"3/6+2/4", "Ответ: 1"},
                                                        {"1/5+9/5", "Ответ: 2"},
                                                        {"9/5+1/5", "Ответ: 2"},
                                                        {"6/3+1/9", "Ответ: 19/9"},
                                                        {"1/9+6/3", "Ответ: 19/9"},
                                                        {"8/6+5/9", "Ответ: 17/9"},
                                                        {"5/9+8/6", "Ответ: 17/9"},
                                                        {"25/3+1/2", "Ответ: 53/6"},
                                                        {"1/2+25/3", "Ответ: 53/6"},
                                                        {"1/3+1/4", "Ответ: 7/12"},
                                                        {"1/4+1/3", "Ответ: 7/12"},
                                                        {"3/7+2/3", "Ответ: 23/21"},
                                                        {"2/3+3/7", "Ответ: 23/21"},
                                                        {"1/2-3/4", "Ответ: -1/4"},
                                                        {"3/4-1/2", "Ответ: 1/4"},
                                                        {"9/5-1/5", "Ответ: 8/5"},
                                                        {"1/5-9/5", "Ответ: -8/5"},
                                                        {"1/2-1/2", "Ответ: 0"},
                                                        {"1/2-2/4", "Ответ: 0"},
                                                        {"2/4-1/2", "Ответ: 0"},
                                                        {"1/2-3/6", "Ответ: 0"},
                                                        {"3/6-2/4", "Ответ: 0"},
                                                        {"1/5-9/5", "Ответ: -8/5"},
                                                        {"9/5-1/5", "Ответ: 8/5"},
                                                        {"6/3-1/9", "Ответ: 17/9"},
                                                        {"1/9-6/3", "Ответ: -17/9"},
                                                        {"8/6-5/9", "Ответ: 7/9"},
                                                        {"5/9-8/6", "Ответ: -7/9"},
                                                        {"25/3-1/2", "Ответ: 47/6"},
                                                        {"1/2-25/3", "Ответ: -47/6"},
                                                        {"1/3-1/4", "Ответ: 1/12"},
                                                        {"1/4-1/3", "Ответ: -1/12"},
                                                        {"3/7-2/3", "Ответ: -5/21"},
                                                        {"2/3-3/7", "Ответ: 5/21"},
};

void test_programm(){
   Formalization_data tested_space;

   for (auto test_set: tested_sets_GCD_and_LCM){
        assert (tested_space.work(test_set.first) == test_set.second);
   }
   cout<<"Тесты на НОД и НОК пройдены"<<endl;

   for (auto test_set: tested_sets_fractions){
        assert (tested_space.work(test_set.first) == test_set.second);
   }
   cout<<"Тесты на дроби пройдены"<<endl;
   cout<<"Все тесты пройдены"<<endl;
}
int main() {
    cout<<"Начало работы программы"<<endl;
	cout<<"Примеры ввода:"<<endl<<"2 128"<<endl<<"1/2+3/4"<<endl;
    Formalization_data a;
    test_programm();
    
    return 0; 
}
