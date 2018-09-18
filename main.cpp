#include <bits/stdc++.h>

using namespace std;

// 1
void z_function(string &pattern, vector <int> &z) {
    z.resize(pattern.length(), 0);
    int left = 0, right = 0;
    for(int i = 1; i < pattern.length(); i++) {
        if(i <= right)
            z[i] = min(z[i - left], right - i + 1);

        while(i + z[i] < pattern.length() && pattern[i + z[i]] == pattern[z[i]])
            z[i]++;

        if(i + z[i] - 1 > right) {
            left = i;
            right = i + z[i] - 1;
        }
    }
}

//2
void prefix_func(string &s, vector <int> &p) {
    p.resize(s.length(), 0);
    for(int i = 1; i < p.size(); i++) {
        int j = p[i - 1];
        while(j && s[j] != s[i])
            j = p[j - 1];

        if(s[j] == s[i])
            p[i] = j + 1;
    }
}

//3
vector <int> z_to_p(vector <int>& z) {
    vector <int> p(z.size(), 0);
    for(int i = 0, x = 0; i < z.size(); i++, x = i + z[i] - 1)
        p[x] = max(p[x], z[i]);
}

//4
vector <int> p_to_z(vector <int>& p) {
    vector <int> z(p.size(), 0);
    for(int i = 0, x = 0; i < p.size(); i++, x = i - p[i] + 1)
        z[x] = max(z[x], p[i]);
}

// возвращает значение префикс функции для элемента c, если префикс функция элемента до него равна j
int local_prefix_func(string& s, vector <int> &p, char c, int j) {
    while(j && c != s[j])
        j = p[j - 1];

    if(c == s[j])
        return j + 1;

    return 0;
}

//5
// просто подбираем символ с подходящим значением префикс-функции
string min_lex(vector <int>& p) {
    if(!p.size())
        return "";
    string s = "a";
    for(int i = 1; i < p.size(); i++) {
        for(int j = 0; j < 26; j++) {
            char c = 'a' + j;
            if(local_prefix_func(s, p, c, p[i - 1]) == p[i]) {
                s += c;
                break;
            }
        }
    }
    return s;
}

int main() {
    vector <int> p;
    int x;
    while(cin >> x && x != -1)
        p.emplace_back(x);
    cout << min_lex(p);
    return 0;
}