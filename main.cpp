#include <vector>
#include <string>
#include <iostream>

using namespace std;

void z_function(const string &pattern, vector<int> &z_values) {
    z_values.resize(pattern.length(), 0);
    int left = 0;
    int right = 0;
    for(int i = 1; i < pattern.length(); i++) {
        // i is inside of the block where we have information about zet function.
        if(i <= right) {
            z_values[i] = min(z_values[i - left], right - i + 1);
        }
        // looking up new symbols.
        while(i + z_values[i] < pattern.length() && pattern[i + z_values[i]] == pattern[z_values[i]]) {
            ++z_values[i];
        }
        // updating block information.
        if(i + z_values[i] - 1 > right) {
            left = i;
            right = i + z_values[i] - 1;
        }
    }
}

void prefix_func(const string &pattern, vector<int> &prefix_values) {
    prefix_values.resize(pattern.length(), 0);
    for(int i = 1; i < prefix_values.size(); i++) {
        int j = prefix_values[i - 1];
        // jumping back on prefixes of prefixes of prefixes etc.
        while(j && pattern[j] != pattern[i]) {
            j = prefix_values[j - 1];
        }

        if(pattern[j] == pattern[i]) {
            prefix_values[i] = j + 1;
        }
    }
}

// Transforms zet function  to prefix function.
vector<int> z_to_p(const vector<int> &z_values) {
    vector<int> prefix_values(z_values.size(), 0);
    for(int i = 1, prefix_position = 0; i < z_values.size(); i++) {
        prefix_position = i + max(z_values[i] - 1, 0);
        prefix_values[prefix_position] = max(prefix_values[prefix_position], z_values[i]);
    }
    int current_maximum = 0;
    for(int i = prefix_values.size() - 1; i > - 1; i--) {
        current_maximum = max(prefix_values[i], current_maximum);
        prefix_values[i] = current_maximum;
        --current_maximum;
    }
    return prefix_values;
}

// Transforms prefix function to zet function.
vector<int> p_to_z(const vector<int> &prefix_values) {
    vector<int> z_values(prefix_values.size(), 0);
    for(int i = 0, z_position = 0; i < prefix_values.size(); i++) {
        z_position = i - max(prefix_values[i] - 1, 0);
        z_values[z_position] = max(z_values[z_position], prefix_values[i]);
    }
    return z_values;
}

// Returns the value of prefix_function for symbol 'c' if 'last_prefix' is the last prefix function value.
int local_prefix_func(const string &pattern, const vector<int> &prefix_values, char c, int last_prefix) {
    while(last_prefix && c != pattern[last_prefix]) {
        last_prefix = prefix_values[last_prefix - 1];
    }

    if(c == pattern[last_prefix]) {
        return last_prefix + 1;
    }

    return 0;
}

// Constructs a lexicographically minimal string for the given values of the correct prefix function.
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