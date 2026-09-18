#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <queue>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

bool hasCorrectLength(const string &in)
{
    return in.size() == 4;
}

bool dontStartWithZero(const string &in)
{
    return in[0] != '0';
}

bool onlyDigits(const string &in)
{
    for (auto &sym : in)
    {
        if (!isdigit(sym))
            return false;
    }
    return true;
}

bool hasUniqueDigits(const string &in)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = i + 1; j < 4; j++)
        {
            if (in[i] == in[j])
                return false;
        }
    }
    return true;
}

void check(string &in)
{
    bool flag = true;
    while (flag)
    {
        cout << "Введите число: ";
        getline(cin, in);
        flag = false;
        if (!hasCorrectLength(in) || !dontStartWithZero(in))
        {
            cout << "Число должно состоять из 4-х цифр и не начинаться с 0\n";
            flag = true;
            continue;
        }
        if (!onlyDigits(in))
        {
            cout << "Число должно состоять из 4-х цифр\n";
            flag = true;
            continue;
        }
        if (!hasUniqueDigits(in))
        {
            cout << "Число должно состоять из 4-х неповторяющихся цифр\n";
            flag = true;
        }
    };
}

int isBullCowOrNone(const string &in, char c, int index)
{
    if (in[index] == c)
    {
        return 2; // Бык
    }
    for (const auto &el : in)
    {
        if (el == c)
            return 1; // Корова
    }
    return 0; // Ничего
}

void placeKnownBull(const vector<char> &bulls, int i, string &select, bool &flag)
{
    if (bulls[i] != 0)
    {
        select.push_back(bulls[i]);
        flag = true;
    }
}

void placeFromCows(map<char, set<int>> &cows, int i, string &select, bool &flag)
{
    for (auto &el : cows)
    {
        if ((count(el.second.begin(), el.second.end(), i) == 0) &&
            (count(select.begin(), select.end(), el.first) == 0))
        {
            select.push_back(el.first);
            el.second.insert(i);
            flag = true;
            break;
        }
    }
}

void placeFromBullsFallback(const vector<char> &bulls,
                            const map<char, set<int>> &cows,
                            int i, string &select)
{
    if (bulls[i] != 0)
    {
        select.push_back(bulls[i]);
        return;
    }
    for (int j = 0; j < 10; j++)
    {
        if ((count(bulls.begin(), bulls.end(), j + 48) == 0) &&
            (count_if(cows.begin(), cows.end(),
                      [&](const pair<char, set<int>> &el)
                      {
                          return char(j + 48) == el.first;
                      }) == 0))
        {
            select.push_back(bulls[j]);
            break;
        }
    }
}

void placeNewDigit(queue<char> &digits, string &select)
{
    select.push_back(digits.front());
    digits.pop();
}

string buildGuess(queue<char> &digits,
                  map<char, set<int>> &cows,
                  const vector<char> &bulls)
{
    string select;
    for (int i = 0; i < 4; i++)
    {
        bool flag = false;

        if (digits.empty() && bulls[i] != 0)
        {
            placeKnownBull(bulls, i, select, flag);
            continue;
        }

        placeFromCows(cows, i, select, flag);
        if (flag)
            continue;

        if (digits.empty())
        {
            placeFromBullsFallback(bulls, cows, i, select);
        }
        else
        {
            placeNewDigit(digits, select);
        }
    }
    return select;
}

void handleBull(char digit, int i, vector<char> &bulls,
                map<char, set<int>> &cows)
{
    bulls[i] = digit;
    for (auto &el : cows)
    {
        el.second.insert(i);
        if (el.second.size() == 3)
        {
            for (int j = 0; j < 4; j++)
            {
                if (count(el.second.begin(), el.second.end(), j) == 0)
                {
                    if (bulls[j] == 0)
                        bulls[j] = el.first;
                }
            }
        }
    }
    auto it = find_if(cows.begin(), cows.end(),
                      [&](const pair<char, set<int>> &p)
                      { return digit == p.first; });
    if (it != cows.end())
        cows.erase(it);
}

void handleCow(char digit, int i, vector<char> &bulls,
               map<char, set<int>> &cows)
{
    cows[digit].insert(i);
    for (int j = 0; j < 4; j++)
    {
        if (bulls[j] != 0)
        {
            cows[digit].insert(j);
        }
    }
    if (cows[digit].size() == 3)
    {
        for (int j = 0; j < 4; j++)
        {
            if (count(cows[digit].begin(), cows[digit].end(), j) == 0)
            {
                if (bulls[j] == 0)
                    bulls[j] = digit;
                for (auto &el : cows)
                {
                    el.second.insert(j);
                }
                auto it = find_if(cows.begin(), cows.end(),
                                  [&](const pair<char, set<int>> &p)
                                  {
                                      return digit == p.first;
                                  });
                if (it != cows.end())
                    cows.erase(it);
            }
        }
    }
}

void processGuess(const string &in, const string &select,
                  vector<char> &bulls, map<char, set<int>> &cows)
{
    for (int i = 0; i < 4; i++)
    {
        int res = isBullCowOrNone(in, select[i], i);
        switch (res)
        {
        case 2:
            cout << "Б";
            handleBull(select[i], i, bulls, cows);
            break;
        case 1:
            cout << "К";
            handleCow(select[i], i, bulls, cows);
            break;
        default:
            cout << "Н";
        }
    }
    cout << endl;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    system("chcp 65001 > nul");

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif

    cout << "Начало работы программы" << endl;
    setlocale(LC_ALL, "ru");
    string in;
    check(in);

    queue<char> digits({'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'});
    map<char, set<int>> cows;
    vector<char> bulls(4);

    while (count(bulls.begin(), bulls.end(), 0) != 0)
    {
        string select = buildGuess(digits, cows, bulls);
        cout << select << endl;
        processGuess(in, select, bulls, cows);
    };

    for (const auto &el : bulls)
    {
        cout << el;
    }
    cout << endl
         << "ББББ" << endl;
    cout << "Конец работы программы";
}