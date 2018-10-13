#include <bits/stdc++.h>
using namespace std;

struct vertex {
    unordered_map <char, size_t> child;
    size_t number;
    int left;
    int right;
    size_t parent;
    size_t link;

    vertex(size_t num, int l, int r, size_t p, size_t lin) {
        number = num;
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
        vertices.emplace_back(vertex(0, 0, 0, 0, 0));
        vertices_count = 1;
    }

    SuffixTree(string &built_string, char delimiter) : SuffixTree(built_string) {
        work_string += delimiter;
    }

    void build_tree() {
        for(size_t i = 0; i < size; ++i) {
            make_phase(i);
        }
    }

    void dfs(size_t ver, int &number, size_t string_size, size_t valid_parent, bool to_print) {
        vertex *current = &vertices[ver];
        if(!(ver == 0 || (current->left < string_size && current->right > string_size))) {
            ++number;
            valid_parent = number;
            if(to_print) {
                // to fix
                cout << current->parent << " ";
                if(current->left < string_size) {
                    cout << 0 << " " << current->left << " " << current->right << endl;
                }
                else {
                    cout << 1 << " " << current->left - string_size << " " << current->right - string_size << endl;
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
    vector<pair <size_t, int> > stop_point;
    size_t size;
    size_t vertices_count;

    void make_phase(size_t phase) {
        stop_point.emplace_back(make_pair(0, 0));
        char c = work_string[phase];

        for(size_t suffix = 0; suffix <= phase; ++suffix) {
            size_t vertex_number = stop_point[suffix].first;
            int edge_position = stop_point[suffix].second;
            vertex *current = &vertices[vertex_number];

            // 1 case
            if(vertex_number && current->child.empty() && edge_position == current->right - current->left) {
                // seems to be useless
                ++stop_point[suffix].second;
                ++current->right;
            }
            else if(edge_position == current->right - current->left) {
                if(current->child.find(c) == current->child.end()) {
                    stop_point[suffix] = make_pair(vertices_count, 1);
                    vertices.emplace_back(vertex(vertices_count, phase, phase + 1, vertex_number, 0));
                    // we cant use pointer here, because vector can change his size (and memory location)
                    vertices[vertex_number].child[c] = vertices_count;
                    ++vertices_count;
                }
                else {
                    stop_point[suffix] = make_pair(current->child[c], 1);
                }
            }
            else {
                if(c == work_string[current->left + edge_position]) {
                    ++stop_point[suffix].second;
                }
                // experimental split
                else {
                    // extra vertex that splits edge
                    vertices.emplace_back(vertex(vertices_count, current->left, current->left + edge_position, current->parent, 0));
                    current = &vertices[vertex_number];
                    vertices[current->parent].child[work_string[current->left]] = vertices_count;
                    current->parent = vertices_count;
                    current->left += edge_position;
                    ++vertices_count;
                    // new leaf vertex
                    vertices.emplace_back(vertex(vertices_count, phase, phase + 1, vertices_count - 1, 0));
                    vertices[vertices_count - 1].child[c] = vertices_count;
                    stop_point[suffix] = make_pair(vertices_count, 1);
                    ++vertices_count;
                }
            }
        }
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
    mysuf.dfs(0, number, s.size(), 0, false);
    cout << number << endl;
    number = 0;
    mysuf.dfs(0, number, s.size(), 0, true);
    return 0;
}