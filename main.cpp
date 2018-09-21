#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

class Trie {
    // (vertex, char)[coded into 1 int] -> (son of vertex by char)
    unordered_map<int, int> edges;
    // (parent, son)[coded into 1 int] -> (char of edge)
    unordered_map<int, char> char_edges;
    vector<int> parents;
    vector<vector<int> > indices;
    int terminal_index;
    int last_index;

    void add_str(const string &pattern) {
        if(!pattern.length()) {
            return;
        }
        int i = 0;
        for(char c : pattern) {
            int cod = code(i, c);
            if(edges.find(cod) == edges.end()) {
                edges[cod] = last_index;
                vector<int> k;
                indices.emplace_back(k);
                parents.emplace_back(i);
                char_edges[code_int(i, last_index)] = c;
                i = last_index++;
            }
            else {
                i = edges[cod];
            }
        }
        indices[i].emplace_back(terminal_index++);
    }

public:
    Trie(const vector<string> &pattern_list) {
        terminal_index = 0;
        last_index = 1;
        // zero index automatically goes to root
        indices.resize(1);
        // the root is a parent of himself
        parents.resize(1, 0);
        for(const string pattern : pattern_list) {
            add_str(pattern);
        }
    }

    //getters
    int get_ind_size(int i) {
        return indices[i].size();
    }

    vector<int> get_ind(int i) {
        return indices[i];
    }

    int get_par(int i) {
        return parents[i];
    }

    int get_edge(int i, char c) {
        int cod =  code(i, c);
        return edges.find(cod) == edges.end() ? -1 : edges[cod];
    }

    char get_char_edges(int i, int j) {
        return char_edges[code_int(i, j)];
    }

    int get_size() {
        return indices.size();
    }
    // coders
    int code_int(int i, int j) const {
        return (i << 15) + j;
    }

    int code(int i, char c) const {
        return (i << 15) + static_cast<unsigned int>(c);
    }
};


class Automate {
    vector<int> suffix_values;
    vector<int> terminal_values;
    // transit - transitional function
    unordered_map<int, int> transit_table;
    Trie *trie;
    // [current] state [of the automate] - number of some vertex
    int state = 0;
    // it compresses the results given by terminal[suffix link] function
    vector<vector<int> > compressor;

    int suffix_link(int num) {
        if(suffix_values[num] != -1) {
            return suffix_values[num]; 
        }
        int parent = trie->get_par(num);
        if(!parent) {
            return suffix_values[num] = 0;
        }
        return suffix_values[num] = transit(suffix_link(parent), trie->get_char_edges(parent, num));
    }

    int terminal(int num) {
        if(terminal_values[num] != -1) {
            return terminal_values[num];
        }
        if(trie->get_ind_size(suffix_link(num))) {
            terminal_values[num] = suffix_link(num);
        }
        else if(!suffix_link(num)) {
            terminal_values[num] = 0;
        }
        else {
            terminal_values[num] = terminal(suffix_link(num));
        }
        return terminal_values[num];
    }

    int transit(int num, char c) {
        int cod = trie->code(num, c);
        if(transit_table.find(cod) != transit_table.end()) {
            return transit_table[cod];
        }
        if(trie->get_edge(num, c) != -1) {
            transit_table[cod] = trie->get_edge(num, c);
        }
        else if(!num) {
            transit_table[cod] = 0;
        }
        else {
            transit_table[cod] = transit(suffix_link(num), c);
        }
        return transit_table[cod];
    }

public:

    Automate(Trie& trie1) {
        trie = &trie1;
        suffix_values.resize(trie->get_size(), -1);
        terminal_values.resize(trie->get_size(), -1);
        compressor.resize(trie->get_size());
        suffix_values[0] = 0;
    }

    // в русском языке автомат стреляет, поэтому такое название
    vector<int> fire(char c) {
        int cur = transit(state, c);
        state = cur;
        // if compressor[state] is not empty - then result was received previously
        // so we don't need to calculate it again
        if(!compressor[state].empty()) {
            return compressor[state];
        }
        vector<int> list;
        // jumping over terminal links
        // first, of course may be not a terminal one, but size of list'll be empty
        while(cur) {
            for(int i : trie->get_ind(cur)) {
                list.emplace_back(i);
            }
            cur = terminal(cur);
        }
        return compressor[state] = list;
    }

};

int main() {
    // speed-up
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    string text;
    getline(cin, text);
    //каретка головного мозга
    if(text[text.length() - 1] == '\r')
        text.erase(text.end() - 1);

    vector<string> pattern_list;
    // indices[j] is a position of the last symbol of pattern number j in the main-pattern
    vector<int> indices;
    int cur = -1;
    // splits text into a words by '?'
    for(int i = 0; i < text.length();) {
        // new string for trie
        if(text[i] != '?') {
            ++cur;
            pattern_list.emplace_back("");
            pattern_list[cur] += text[i++];
            while(text.length() > i && text[i] != '?') {
                pattern_list[cur] += text[i++];
            }
            indices.emplace_back(i - 1);
        }

        while(i < text.length() && text[i] == '?') {
            ++i;
        }
    }

    int pos = 0;

    Trie bor(pattern_list);
    Automate mate(bor);
    if(pattern_list.size() == 0) {
        for(int i = 0; i < pos - text.length() + 1; i++)
            cout << i << " ";
        return 0;
    }
    pattern_list.clear();

    char c;
    // mappy stores possible positions which main-pattern can begin from
    vector<short int> mappy;
    // determines positions of pattern in the text
    while(cin >> c) {
        vector<int> list = mate.fire(c);
        mappy.emplace_back(0);
        for(int i : list) {
            int position = pos - indices[i];
            if(position < 0)
                continue;
           ++mappy[position];
        }
        ++pos;
    }

    // handles the situation when there are '?' in the end of string
    for(int i = 0; i < mappy.size(); i++) {
        if(mappy[i] == cur + 1 && i + text.length() <= pos) {
            cout << i << " ";
        }
    }
    return 0;
}