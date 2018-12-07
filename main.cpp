#include <iostream>
#include <string>
#include <vector>

using namespace std;

void z_function(const string &pattern, vector<size_t > &z_values) {
    z_values.resize(pattern.length(), 0);
    size_t left = 0;
    size_t right = 0;
    for(size_t i = 1; i < pattern.length(); ++i) {
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

void prefix_func(const string &pattern, vector<size_t> &prefix_values) {
    prefix_values.resize(pattern.length());
    for(size_t i = 1; i < prefix_values.size(); ++i) {
        size_t j = prefix_values[i - 1];
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
void z_to_p(const vector<size_t> &z_values, vector<size_t> &prefix_values) {
    prefix_values.resize(z_values.size());
    for(size_t i = 1, prefix_position = 0; i < z_values.size(); ++i) {
        prefix_position = i + max(static_cast<int>(z_values[i]) - 1, 0);
        prefix_values[prefix_position] = max(prefix_values[prefix_position], z_values[i]);
    }
    int current_maximum = 0;
    for(int i = static_cast<int>(prefix_values.size()) - 1; i > - 1; --i) {
        current_maximum = max(static_cast<int>(prefix_values[i]), current_maximum);
        prefix_values[i] = static_cast<size_t >(current_maximum);
        --current_maximum;
    }
}

// Transforms prefix function to zet function.
void p_to_z(const vector<size_t > &prefix_values, vector<size_t> &z_values) {
    z_values.resize(prefix_values.size());
    for(size_t i = 0, z_position = 0; i < prefix_values.size(); ++i) {
        z_position = i - max(static_cast<int>(prefix_values[i]) - 1, 0);
        z_values[z_position] = max(z_values[z_position], prefix_values[i]);
    }
}

// Returns the value of prefix_function for symbol 'c' if 'last_prefix' is the last prefix function value.
size_t local_prefix_func(const string &pattern, const vector<size_t > &prefix_values, char c, size_t last_prefix) {
    while(last_prefix > 0 && c != pattern[last_prefix]) {
        last_prefix = prefix_values[last_prefix - 1];
    }

    if(c == pattern[last_prefix]) {
        return last_prefix + 1;
    }

    return 0;
}

// Constructs a lexicographically minimal string for the given values of the correct prefix function.
string min_lex(const vector<size_t > &prefix_values) {
    if(prefix_values.empty()) {
        return "";
    }
    string minimal_string = "a";
    for(size_t i = 1; i < prefix_values.size(); ++i) {
        for(size_t j = 0; j < 26; ++j) {
            char c = static_cast<char>(static_cast<int>('a') + j);
            if(local_prefix_func(minimal_string, prefix_values, c, prefix_values[i - 1]) == prefix_values[i]) {
                minimal_string += c;
                break;
            }
        }
    }
    return minimal_string;
}

int main() {
    vector<size_t > z_values;
    size_t x;
    while(cin >> x && x != -1) {
        z_values.emplace_back(x);
    }
    vector<size_t> kke;
    z_to_p(z_values, kke);
    cout << min_lex(kke);
    return 0;
}