#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>

using std::ios_base;
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;

// struct of node in the tree
struct vertex {
    // array for storing symbols
    size_t child[28];
    // left border excluding
    int left;
    // right border on edge - excluding
    int right;
    // distance from root
    int distance;

    size_t parent;
    size_t link;

    vertex(int l, int r, size_t p, size_t lin) {
        left = l;
        right = r;
        parent = p;
        link = lin;
        distance = -1;
        for(int i = 0; i < 28; ++i) {
            child[i] = 0;
        }
    }

    bool is_child(char c) {
        return child[determine(c)] > 0;
    }

    void make_child(char c, size_t value) {
        child[determine(c)] = value;
    }

    size_t get_child(char c) {
        return child[determine(c)];
    }

    // determine the index of char in child array
    int determine(char c) {
        if(c >= 'a') {
            return c - 'a';
        }
        else {
            return 26 + c - '#';
        }
    }

    // length of the edge to that vertex
    int len() {
        return right - left;
    }
};

class SuffixTree {
public:
    SuffixTree(string &built_string) {
        work_string = built_string;
        vertices.emplace_back(vertex(0, 0, 0, 0));
        vertices_count = 1;
        vertices[0].distance = 0;
        previous_vertex_number = 0;
    }

    SuffixTree(string &built_string, char delimiter) : SuffixTree(built_string) {
        work_string += delimiter;
    }

    size_t get_size() {
        return vertices_count;
    }

    // passes the phases of algorithm
    void build_tree() {
        size_t previous = 0;
        for(size_t i = 0; i < work_string.size(); ++i) {
            previous = make_phase(i, previous);
        }
    }

    // visits the tree in  ascending lexicographical order and prints the vertices
    void dfs(size_t ver, size_t &number, size_t fst, size_t scd, size_t parent) {
        vertex *current = &vertices[ver];
        if(ver != 0) {
            ++number;
            printf("%d ", parent);
            // 1 string
            if(current->left >= fst) {
                printf("%d %d %d\n", 1, current->left - fst, current->right - fst);
            }
                // 0 string
            else if(current->right > fst){
                printf("%d %d %d\n", 0, current->left, current->right - scd);
            }
                // hmm, 0 string
            else {
                printf("%d %d %d\n", 0, current->left, current->right);
            }
        }

        parent = number;

        if(current->is_child('#')) {
            dfs(current->get_child('#'), number, fst, scd, parent);
        }

        if(current->is_child('$')) {
            dfs(current->get_child('$'), number, fst, scd, parent);
        }

        for(char i = 'a'; i <= 'z'; ++i) {
            if(current->is_child(i)) {
                dfs(current->get_child(i), number, fst, scd, parent);
            }
        }
    }

private:
    string work_string;
    vector<vertex> vertices;
    size_t vertices_count;
    // number of the vertex where we ended previous phase
    size_t previous_vertex_number;

    // calculates the distance from root to that vertex (without the direct edge to vertex)
    int dist(size_t vertex_number) {
        if(vertices[vertex_number].distance != -1) {
            return vertices[vertex_number].distance;
        }
        else {
            size_t parent = vertices[vertex_number].parent;
            return vertices[vertex_number].distance = dist(parent) + vertices[parent].len();
        }
    }

    size_t make_phase(size_t phase, size_t iterations_begin) {
        char c = work_string[phase];

        size_t suffix_link = previous_vertex_number;
        // the new inserted internal vertex to build a suffix link
        size_t previous_internal = 0;
        // distance from root (summing by all edges except direct edge to vertex)
        int distance = 0;
        // symbols that we haven't passed yet (direct edge to vertex)
        int len = 0;

        // suffix where the new internal vertex was created (automatically added +1 to make if statement without +1)
        size_t internal = 0;

        for(size_t suffix = iterations_begin; suffix <= phase; ++suffix) {
            // we come through suffix link to a desired vertex
            size_t vertex_number = suffix_link;
            vertex *current = &vertices[vertex_number];
            distance = dist(vertex_number);
            len = phase - suffix - distance;

            // we descend until we can
            // we can if length of the current edge is less than symbols that we need to pass (len)
            while(len > current->len()) {
                len -= current->len();
                distance += current->len();
                vertex_number = current->get_child(work_string[suffix + distance]);
                current = &vertices[vertex_number];
            }

            // internal vertex handle
            // in case we have stopped at the end of the edge, so the last symbol we need to check in child of vertex
            if(len == current->len()) {
                // making suffix link for previous memorized internal vertex
                if(previous_internal != 0 && suffix == internal) {
                    vertices[previous_internal].link = vertex_number;
                    previous_internal = 0;
                }
                // current vertex is not a new internal so we don't memorize it
                // (we have already figured out its suffix link, and it won't change)

                if(!current->is_child(c)) {
                    // we make new leaf from existing internal vertex
                    current->make_child(c, vertices_count);
                    vertices.emplace_back(vertex(phase, work_string.size(), vertex_number, 0));
                    ++vertices_count;
                    suffix_link = vertices[vertex_number].link;
                }
                else {
                    previous_vertex_number = current->get_child(c);
                    return suffix;
                }
            }
            else {
                // we have stopped at the middle of the edge, so we need to check symbol
                if(work_string[current->left + len] != c) {
                    // in that case we split the edge, so new internal vertex is created
                    // its number will be - vertices_count

                    // making suffix link for previous memorized internal vertex
                    if(previous_internal != 0 && suffix == internal) {
                        vertices[previous_internal].link = vertices_count;
                    }
                    // we need to calculate its suffix link, so we memorize it
                    previous_internal = vertices_count;
                    internal = suffix + 1;

                    // direct edge to vertex_number was modified, so we need to calculate it again
                    vertices[vertex_number].distance = -1;

                    // hard splitting operation
                    vertices.emplace_back(vertex(current->left, current->left + len, current->parent, 0));
                    current = &vertices[vertex_number];
                    vertices[vertices_count].make_child(work_string[current->left + len], vertex_number);
                    vertices[current->parent].make_child(work_string[current->left], vertices_count);
                    current->parent = vertices_count;
                    current->left += len;
                    ++vertices_count;

                    // new leaf vertex created
                    vertices.emplace_back(vertex(phase, work_string.size(), vertices_count - 1, 0));
                    vertices[vertices_count - 1].make_child(c, vertices_count);
                    ++vertices_count;

                    // we go back in tree to receive a suffix link
                    size_t parent = vertices[previous_internal].parent;
                    suffix_link = vertices[parent].link;
                }
                else {
                    previous_vertex_number = vertex_number;
                    return suffix;
                }
            }
        }
        previous_vertex_number = 0;
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
    cout << mysuf.get_size() << endl;
    mysuf.dfs(0, number, s.size(), t.size(), 0);
    return 0;
}