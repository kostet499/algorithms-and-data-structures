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

    void dfs(size_t ver, int &number, size_t string_size, size_t valid_parent, bool to_print) {
        vertex *current = &vertices[ver];
        if(ver != 0) {
            ++number;
            valid_parent = static_cast<size_t>(number);
            if(to_print) {
                // to fix
                cout << current->parent << " ";
                if(current->right < string_size) {
                    cout << 1 << " " << current->left << " " << current->right << endl;
                }
                else {
                    cout << 0 << " " << current->left << " " << current->right - string_size << endl;
                }
            }
        }

        if(current->child.find('$') != current->child.end()) {
            dfs(current->child['$'], number, string_size, valid_parent, to_print);
        }

        if(current->child.find('#') != current->child.end()) {
            dfs(current->child['#'], number, string_size, valid_parent, to_print);
        }

        for(char i = 'a'; i <= 'z'; ++i) {
            if(current->child.find(i) != current->child.end()) {
                dfs(current->child[i], number, string_size, valid_parent, to_print);
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
        for(size_t suffix = iterations_begin; suffix <= phase; ++suffix) {
            // experimental decent
            size_t vertex_number = 0;
            int len = phase - suffix;
            if(true || suffix_link == 0) {
                vertex *current = &vertices[vertex_number];
                int difference = current->right - current->left;
                while(len > difference) {
                    vertex_number = current->child[work_string[suffix + difference]];
                    current = &vertices[vertex_number];
                    difference += current->right - current->left;
                }
                if(len != difference) {
                    len -= difference - current->right + current->left;
                }
                else {
                    len = 0;
                }
            }
            else {

            }

            vertex *current = &vertices[vertex_number];

            // come to the end of the edge
            if(current->left + len == current->right) {
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

            suffix_link = vertices[vertices[vertex_number].parent].link;
        }
        return phase + 1;
    }
};

int main() {
    string s;
    string t;
    cin >> s >> t;
    string concat = s + t;
    SuffixTree mysuf(concat);
    mysuf.build_tree();
    int number = 0;
    mysuf.dfs(0, number, t.size(), 0, false);
    cout << number + 1 << endl;
    number = 0;
    mysuf.dfs(0, number, t.size(), 0, true);
    return 0;
}