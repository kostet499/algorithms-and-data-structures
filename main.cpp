#include <vector>
#include <string>
#include <iostream>


using namespace std;

// Counts zet function of the given string
void z_function(const string &pattern, vector<int> &z_values) {
    z_values.resize(pattern.length(), 0);
    int left = 0;
    int right = 0;
    for(int i = 1; i < pattern.length(); i++) {
        if(i <= right) {
            z_values[i] = min(z_values[i - left], right - i + 1);
        }

        while(i + z_values[i] < pattern.length() && pattern[i + z_values[i]] == pattern[z_values[i]]) {
            ++z_values[i];
        }

        if(i + z_values[i] - 1 > right) {
            left = i;
            right = i + z_values[i] - 1;
        }
    }
}

//Counts prefix function of the given string
void prefix_func(const string &pattern, vector<int> &prefix_values) {
    prefix_values.resize(pattern.length(), 0);
    for(int i = 1; i < prefix_values.size(); i++) {
        int j = prefix_values[i - 1];

        while(j && pattern[j] != pattern[i]) {
            j = prefix_values[j - 1];
        }

        if(pattern[j] == pattern[i]) {
            prefix_values[i] = j + 1;
        }
    }
}

// Transforms zet function  to prefix function
vector<int> z_to_p(const vector<int> &z_values) {
    vector<int> prefix_values(z_values.size(), 0);
    for(int i = 1, x = 0; i < z_values.size(); i++) {
        x = i + max(z_values[i] - 1, 0);
        prefix_values[x] = max(prefix_values[x], z_values[i]);
    }
    int cur_m = 0;
    for(int i = prefix_values.size() - 1; i > - 1; i--) {
        cur_m = max(prefix_values[i], cur_m);
        prefix_values[i] = cur_m;
        --cur_m;
    }
    return prefix_values;
}

// Transforms prefix function to zet function
vector<int> p_to_z(const vector<int> &prefix_values) {
    vector<int> z_values(prefix_values.size(), 0);
    for(int i = 0, x = 0; i < prefix_values.size(); i++) {
        x = i - max(prefix_values[i] - 1, 0);
        z_values[x] = max(z_values[x], prefix_values[i]);
    }
    return z_values;
}

// возвращает значение префикс функции для элемента c, если префикс функция элемента до него равна j
int local_prefix_func(const string &pattern, const vector<int> &prefix_values, char c, int last_prefix) {
    while(last_prefix && c != pattern[last_prefix]) {
        last_prefix = prefix_values[last_prefix - 1];
    }

    if(c == pattern[last_prefix]) {
        return last_prefix + 1;
    }

    return 0;
}

// Строит лексикографически минимальну строку по заданным значениям префикс фукнции(корректной)
string min_lex(const vector<int> &prefix_values) {
    if(!prefix_values.size()) {
        return "";
    }
    string minimal_string = "a";
    for(int i = 1; i < prefix_values.size(); i++) {
        for(int j = 0; j < 26; j++) {
            char c = 'a' + j;
            if(local_prefix_func(minimal_string, prefix_values, c, prefix_values[i - 1]) == prefix_values[i]) {
                minimal_string += c;
                break;
            }
        }
    }
    return minimal_string;
}

int main() {
    vector<int> z_values;
    int x;
    while(cin >> x && x != -1) {
        z_values.emplace_back(x);
    }
    z_values = z_to_p(z_values);
    cout << min_lex(z_values);
    return 0;
}