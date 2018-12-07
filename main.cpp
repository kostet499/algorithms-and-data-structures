#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

class Trie {

public:
    explicit Trie(const vector<string> &pattern_list) : terminal_index(0), last_index(1), indices(0),
        parents(0), char_edges(1) {
        for(const auto &pattern : pattern_list) {
            add_str(pattern);
        }
    }

    //getters
    bool is_terminal(int i) const {
        return indices[i].empty();
    }

    const vector<int>& get_terminal_indices(int i) const {
        return indices[i];
    }

    int get_parent(int i) const {
        return parents[i];
    }

    int get_edge(int i, char c) {
        int key =  encode(i, c);
        return edges.find(key) == edges.end() ? no_edge : edges[key];
    }

    char get_char_edges(int vertex) const {
        return char_edges[vertex];
    }

    size_t get_size() const {
        return indices.size();
    }

    // makes a key from int and char, that will be used in map
    int encode(int vertex, char c) const {
        return (vertex << 15) + static_cast<unsigned int>(c);
    }

private:
    void add_str(const string &pattern) {
        if(!pattern.length()) {
            return;
        }
        int i = 0;
        for(auto &c : pattern) {
            int key = encode(i, c);
            if(edges.find(key) == edges.end()) {
                edges[key] = last_index;
                char_edges.emplace_back(c);
                indices.emplace_back();
                parents.emplace_back(i);
                i = last_index++;
            }
            else {
                i = edges[key];
            }
        }
        indices[i].emplace_back(terminal_index++);
    }

private:
    // (vertex, char)[keyd into 1 int] -> (son of vertex by char)
    unordered_map<int, int> edges;
    // char of edge to the vertex
    vector<char> char_edges;
    vector<int> parents;
    vector<vector<int> > indices;
    int terminal_index;
    int last_index;
public:
    static const int no_edge = -1;
};

class Automate {

public:
    explicit Automate(Trie &trie1) : trie(trie1) {
        suffix_values.resize(trie.get_size(), Trie::no_edge);
        terminal_values.resize(trie.get_size(), Trie::no_edge);
        compressor.resize(trie.get_size());
        visited.resize(trie.get_size(), false);
        suffix_values[0] = 0;
    }

    const vector<int> &process_symbol(char c) {
        state = transit(state, c);
        return get_result(state);
    }

private:
    int suffix_link(int vertex) {
        if(suffix_values[vertex] != Trie::no_edge) {
            return suffix_values[vertex];
        }
        int parent = trie.get_parent(vertex);
        if(!parent) {
            return suffix_values[vertex] = 0;
        }
        return suffix_values[vertex] = transit(suffix_link(parent), trie.get_char_edges(vertex));
    }

    int terminal(int vertex) {
        if(terminal_values[vertex] != Trie::no_edge) {
            return terminal_values[vertex];
        }
        if(trie.is_terminal(suffix_link(vertex))) {
            terminal_values[vertex] = suffix_link(vertex);
        }
        else if(!suffix_link(vertex)) {
            terminal_values[vertex] = 0;
        }
        else {
            terminal_values[vertex] = terminal(suffix_link(vertex));
        }
        return terminal_values[vertex];
    }

    int transit(int vertex, char c) {
        int key = trie.encode(vertex, c);
        if(transit_table.find(key) != transit_table.end()) {
            return transit_table[key];
        }
        if(trie.get_edge(vertex, c) != Trie::no_edge) {
            transit_table[key] = trie.get_edge(vertex, c);
        }
        else if(!vertex) {
            transit_table[key] = 0;
        }
        else {
            transit_table[key] = transit(suffix_link(vertex), c);
        }
        return transit_table[key];
    }

    // uses previous results to figure out the result for current request
    const vector<int> &get_result(int vertex) {
        if(visited[vertex]) {
            return compressor[vertex];
        }
        visited[vertex] = true;
        compressor[vertex] = get_result(terminal(vertex));
        for(auto i : trie.get_terminal_indices(vertex)) {
            compressor[vertex].emplace_back(i);
        }
        return compressor[vertex];
    }

private:
    vector<int> suffix_values;
    vector<int> terminal_values;
    // transit - transitional function
    unordered_map<int, int> transit_table;
    Trie &trie;
    // [current] state [of the automate] - number of some vertex
    int state = 0;
    // it compresses the results given by terminal[suffix link] function
    vector<vector<int> > compressor;
    vector<bool> visited;
};

// splits pattern into a words by symbol
void split_pattern(string &pattern, vector<string> &word_list, vector<int> &indices, char symbol) {
    int cur = -1;
    for(size_t i = 0; i < pattern.length();) {
        if(pattern[i] != symbol) {
            ++cur;
            word_list.emplace_back("");
            word_list[cur] += pattern[i++];
            while(pattern.length() > i && pattern[i] != symbol) {
                word_list[cur] += pattern[i++];
            }
            indices.emplace_back(i - 1);
        }

        while(i < pattern.length() && pattern[i] == symbol) {
            ++i;
        }
    }
}

int main() {
    // speed-up
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    string pattern;
    cin >> pattern;

    vector<string> word_list;
    // indices[j] is a position of the last symbol of pattern number j in the main-pattern
    vector<int> indices;
    split_pattern(pattern, word_list, indices, '?');

    int pos = 0;
    Trie bor(word_list);
    Automate mate(bor);

    char c;
    // mappy stores possible positions which main-pattern can begin from
    vector<short int> mappy;
    // determines positions of pattern in the text
    while(cin >> c && c != '#') {
        vector<int> list = mate.process_symbol(c);
        mappy.emplace_back(0);
        for(auto i : list) {
            int position = pos - indices[i];
            if(position < 0)
                continue;
           ++mappy[position];
        }
        int position_to_check = pos - static_cast<int>(pattern.length()) + 1;
        if(position_to_check > -1 && mappy[position_to_check] == word_list.size()) {
            cout << position_to_check << " ";
        }
        ++pos;
    }

    return 0;
}