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

    int len() {
        return right - left;
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

    // test functions
    void show_internal_links() {
        int i = 0;
        for(auto vert : vertices) {
            cout << i << " " << vert.link << endl;
            ++i;
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
        // the new inserted internal vertex to build a suffix link
        size_t previous_internal = 0;
        // distance from root (summing by all edges)
        int distance = 0;
        // position on current edge
        int len = 0;

        for(size_t suffix = iterations_begin; suffix <= phase; ++suffix) {
            //suffix_link = 0;
            // we are in the root, so distance from root to root is 0, len is to be counted
            if(suffix_link == 0) {
                distance = 0;
                len = phase - suffix;
            }

            size_t vertex_number = suffix_link;
            vertex *current = &vertices[vertex_number];
            while(len > current->len()) {
                len -= current->len();
                distance += current->len();
                assert(current->child.count(work_string[suffix + distance]));
                vertex_number = current->child[work_string[suffix + distance]];
                current = &vertices[vertex_number];
            }

            // invariant of cycle: len + distance = phase  - suffix
            assert(len + distance == phase - suffix);

            // internal vertex handle
            if(len == current->len()) {
                if(current->child.find(c) == current->child.end()) {
                    if(previous_internal != 0) {
                        vertices[previous_internal].link = vertex_number;
                        previous_internal = 0;
                    }
                    current->child[c] = vertices_count;
                    vertices.emplace_back(vertex(phase, work_string.size(), vertex_number, 0));
                    ++vertices_count;
                    suffix_link = vertices[vertex_number].link;
                    // implicit shit
                    len = vertices[suffix_link].len();
                    distance = phase - suffix - len;
                }
                else {
                    if(previous_internal != 0) {
                        vertices[previous_internal].link = vertex_number;
                        previous_internal = 0;
                    }
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

                    size_t parent = vertices[previous_internal].parent;
                    suffix_link = vertices[parent].link;
                    // we go back, so distance is changed -> len also changed
                    distance -= vertices[parent].len();
                    len += vertices[parent].len();
                }
                else {
                    return suffix;
                }
            }
            // each phase our suffix is incremented, so out length should be decremented
            --distance;
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
    // s = "asdjasfhaslhfjkshahsjsssssssssaashdajshdkjshadhhhhhhhhhhhhhhahdshdshshadjhsajkhdkjsfhkjh$";
    // t = "sadjsasdjklsakdahaaaaaaaaaaaasjdkajsjdsjssssssssss#";
    cin >> s >> t;
    string concat = s + t;
    SuffixTree mysuf(concat);
    mysuf.build_tree();
    size_t number = 0;
    mysuf.dfs(0, number, s.size(), t.size(), number, false);
    cout << number + 1 << endl;
    number = 0;
    mysuf.dfs(0, number, s.size(), t.size(), number, true);
    //mysuf.show_internal_links();
    return 0;
}