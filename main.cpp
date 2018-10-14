#include <bits/stdc++.h>
using namespace std;

struct vertex {
    unordered_map <char, size_t> child;
    int left;
    int right;
    size_t parent;
    size_t link;

    vertex(int l, int r, size_t p, size_t lin) {
        left = l;
        right = r;
        parent = p;
        link = lin;
    }
};

class SuffixTree {
public:
    SuffixTree(string &built_string) {
        work_string = built_string;
        size = built_string.size();
        vertices.emplace_back(vertex(0, 0, 0, 0));
        vertices_count = 1;
    }

    SuffixTree(string &built_string, char delimiter) : SuffixTree(built_string) {
        work_string += delimiter;
    }

    void build_tree() {
        size_t previous = 0;
        for(size_t i = 0; i < size; ++i) {
            previous = make_phase(i, previous);
        }
    }

    void dfs(size_t ver, size_t &number, size_t fst, size_t scd, size_t parent, bool to_print) {
        vertex *current = &vertices[ver];
        if(ver != 0) {
            ++number;
            if(to_print) {
                // to fix
                cout << parent << " ";
                // 1 string
                if(current->left >= fst) {
                    cout << 1 << " " << current->left - fst << " " << current->right - fst << endl;
                }
                    // 0 string
                else if(current->right > fst){
                    cout << 0 << " " << current->left << " " << current->right - scd << endl;
                }
                    // hmm, 0 string
                else {
                    cout << 0 << " " << current->left << " " << current->right << endl;
                }
            }
        }

        parent = number;
        if(current->child.find('#') != current->child.end()) {
            dfs(current->child['#'], number, fst, scd, parent, to_print);
        }

        if(current->child.find('$') != current->child.end()) {
            dfs(current->child['$'], number, fst, scd, parent, to_print);
        }

        for(char i = 'a'; i <= 'z'; ++i) {
            if(current->child.find(i) != current->child.end()) {
                dfs(current->child[i], number, fst, scd, parent, to_print);
            }
        }
    }

private:
    string work_string;
    vector<vertex> vertices;
    size_t size;
    size_t vertices_count;

    size_t make_phase(size_t phase, size_t iterations_begin) {
        char c = work_string[phase];

        size_t suffix_link = 0;
        size_t previous_internal = 0;
        int len = 0;
        char symbol;
        for(size_t suffix = iterations_begin; suffix <= phase; ++suffix) {
            // experimental decent
            size_t vertex_number = 0;
            if(suffix_link == 0) {
                len = phase - suffix;
                vertex *current = &vertices[vertex_number];
                int difference = current->right - current->left;
                while(len > difference) {
                    symbol = work_string[suffix + difference];
                    assert(current->child.find(symbol) != current->child.end());
                    vertex_number = current->child[symbol];
                    current = &vertices[vertex_number];
                    difference += current->right - current->left;
                }
                len -= difference - current->right + current->left;
            }
            else {
                // suppose len stays the same
                vertex_number = vertices[suffix_link].child[symbol];
                vertex *current = &vertices[vertex_number];
                int difference = current->right - current->left;
                while(len > difference) {
                    symbol = work_string[suffix + difference];
                    assert(current->child.find(symbol) != current->child.end());
                    vertex_number = current->child[symbol];
                    current = &vertices[vertex_number];
                    difference += current->right - current->left;
                }
                len -= difference - current->right + current->left;
            }

            vertex *current = &vertices[vertex_number];

            // come to the end of the edge
            if(vertex_number == 0 || current->left + len == current->right) {
                if(current->child.find(c) == current->child.end()) {
                    if(previous_internal != 0) {
                        vertices[previous_internal].link = vertex_number;
                    }
                    previous_internal = vertex_number;
                    current->child[c] = vertices_count;
                    vertices.emplace_back(vertex(phase, work_string.size(), vertex_number, 0));
                    ++vertices_count;
                }
                else {
                    return suffix;
                }
            }
            else {
                // experimental split
                if(work_string[current->left + len] != c) {
                    if(previous_internal != 0) {
                        vertices[previous_internal].link = vertices_count;
                    }
                    previous_internal = vertices_count;
                    // new internal vertex with number "vertices_count"
                    vertices.emplace_back(vertex(current->left, current->left + len, current->parent, 0));
                    current = &vertices[vertex_number];
                    vertices[vertices_count].child[work_string[current->left + len]] = vertex_number;
                    vertices[current->parent].child[work_string[current->left]] = vertices_count;
                    current->parent = vertices_count;
                    current->left += len;
                    ++vertices_count;
                    // new leaf vertex
                    vertices.emplace_back(vertex(phase, work_string.size(), vertices_count - 1, 0));
                    vertices[vertices_count - 1].child[c] = vertices_count;
                    ++vertices_count;
                }
                else {
                    return suffix;
                }
            }

            suffix_link = vertices[vertices[previous_internal].parent].link;
        }
        return phase + 1;
    }
};

int main() {
    cin.tie(0);
    cout.tie(0);
    ios_base::sync_with_stdio(0);
    string s;
    string t;
    cin >> s >> t;
    string concat = s + t;
    SuffixTree mysuf(concat);
    mysuf.build_tree();
    size_t number = 0;
    mysuf.dfs(0, number, s.size(), t.size(), number, false);
    cout << number + 1 << endl;
    number = 0;
    mysuf.dfs(0, number, s.size(), t.size(), number, true);
    return 0;
}