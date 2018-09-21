#include <iostream>
#include <vector>

using namespace std;

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

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    string pattern;
    getline(cin, pattern, '\n');
    vector<int> z_values;
    pattern += '#';
    char c;
    int size = pattern.size() - 1;
    z_function(pattern, z_values);

    int cur = 0, left = 0, right = 0, next = size - 1;
    vector<int> z_cycle(size, 0);
    vector<char> char_cycle(size);
    for(int i = 0; i < size - 1; i++) {
        if (!(cin >> char_cycle[i])) {
            return 0;
        }
    }

    int i = 0;
    while(cin >> char_cycle[next]) {
        z_cycle[next] = 0;
        cur = i % size;
        if(i <= right) {
            z_cycle[cur] = min(z_values[i - left], right - i + 1);
        }

        while(z_cycle[cur] < size && char_cycle[(i + z_cycle[cur]) % size] == pattern[z_cycle[cur]]) {
            ++z_cycle[cur];
        }

        if(i + z_cycle[cur] - 1 > right) {
            left = i;
            right = i + z_cycle[cur] - 1;
        }

        if(z_cycle[cur] == size) {
            cout << i << " ";
        }

        ++i;
        next = (next + 1) % size;
    }
    return 0;
}