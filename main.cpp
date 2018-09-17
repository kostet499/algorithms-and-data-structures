#include <bits/stdc++.h>

using namespace std;

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

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    string pattern;
    getline(cin, pattern, '\n');
    vector <int> z;
    pattern += '#';
    char c;
    int size = pattern.size() - 1;
    z_function(pattern, z);

    int cur = 0, left = 0, right = 0, emp = size - 1;
    vector <int> zf(size, 0);
    vector <char> cf(size);
    for(int i = 0; i < size - 1; i++) {
        if(!(cin >> cf[i]))
            return 0;
        zf[i] = z[i + size + 1];
    }
    int i = 0;
    while(cin >> cf[emp]) {
        zf[emp] = 0;
        cur = i % size;
        if(i <= right)
            zf[cur] = min(z[i - left], right - left + 1);

        while(zf[cur] < size && cf[(i + zf[cur]) % size] == pattern[zf[cur]])
            zf[cur]++;

        if(i + zf[cur] - 1 > right) {
            left = i;
            right = i + zf[cur] - 1;
        }
        if(zf[cur] == size)
            cout << i << " ";
        i++;
        emp = (emp + 1) % size;
    }
    return 0;
}