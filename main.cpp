#include <bits/stdc++.h>
using namespace std;

struct vertex {
    unordered_map <char, size_t> child;
    int left;
    int right;
    int distance;

    size_t parent;
    size_t link;

    vertex(int l, int r, size_t p, size_t lin) {
        left = l;
        right = r;
        parent = p;
        link = lin;
        distance = -1;
    }

    int len() {
        return right - left;
    }
};

class SuffixTree {
public:
    SuffixTree(string &built_string, bool optimized) {
        work_string = built_string;
        size = built_string.size();
        vertices.emplace_back(vertex(0, 0, 0, 0));
        vertices_count = 1;
        vertices[0].distance = 0;
        this->optimized = optimized;
    }

    SuffixTree(string &built_string, bool optimized, char delimiter) : SuffixTree(built_string, optimized) {
        work_string += delimiter;
    }

    size_t get_size() {
        return vertices_count;
    }

    void build_tree() {
        size_t previous = 0;
        for(size_t i = 0; i < size; ++i) {
            try {
                previous = make_phase(i, previous);
            }
            catch(int e) {
                if(e == 1) {
                    cout << "distance < 0 but not a root";
                }
                else if(e == 0) {
                    cout << "no such symbol";
                }
                else if(e == 2) {
                    cout << "mistake in invariant";
                }
                else if(e == 3) {
                    cout << "distance < 0 after parent";
                }
                cout << endl;
                return;
            }
        }
    }

    void dfs(size_t ver, size_t &number, size_t fst, size_t scd, size_t parent) {
        vertex *current = &vertices[ver];
        if(ver != 0) {
            ++number;
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

        parent = number;
        if(current->child.find('#') != current->child.end()) {
            dfs(current->child['#'], number, fst, scd, parent);
        }

        if(current->child.find('$') != current->child.end()) {
            dfs(current->child['$'], number, fst, scd, parent);
        }

        for(char i = 'a'; i <= 'z'; ++i) {
            if(current->child.find(i) != current->child.end()) {
                dfs(current->child[i], number, fst, scd, parent);
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

    void compare(SuffixTree &object) {
        for(int i = 0; i < min(object.result_stack.size(), this->result_stack.size()); i++) {
           if(object.result_stack[i] != this->result_stack[i]) {
               if(i > 0) {
                   cout << "last equal: " << object.result_stack[i - 1] << endl;
               }
               cout << "this obj says: " << this->result_stack[i] << endl;
               cout << "not this objt: " << object.result_stack[i] << endl;
               return;
           }
        }
    }

private:
    string work_string;
    vector<vertex> vertices;
    vector<size_t> result_stack;
    size_t size;
    size_t vertices_count;
    bool optimized;

    int dist(size_t vertex_number) {
        if(vertices[vertex_number].distance != -1) {
            return vertices[vertex_number].distance;
        }
        else {
            size_t parent = vertices[vertex_number].parent;
            return dist(parent) + vertices[parent].len();
        }
    }

    size_t make_phase(size_t phase, size_t iterations_begin) {
        char c = work_string[phase];

        size_t suffix_link = 0;
        // the new inserted internal vertex to build a suffix link
        size_t previous_internal = 0;
        // distance from root (summing by all edges)
        int distance = 0;
        // symbols that we haven't passed yet
        int len = 0;

        // exp
        size_t internal = 0;

        for(size_t suffix = iterations_begin; suffix <= phase; ++suffix) {
            //suffix_link = 0;
            // we are in the root, so distance from root to root is 0, len is to be counted
            if(distance < 0 && suffix_link == 0) {
                throw 1;
            }

            if(!optimized) {
                suffix_link = 0;
            }

            size_t vertex_number = suffix_link;
            vertex *current = &vertices[vertex_number];
            distance = dist(vertex_number);
            len = phase - suffix - distance;
            while(len > current->len()) {
                len -= current->len();
                distance += current->len();
                if(current->child.count(work_string[suffix + distance]) == 0) {
                    throw 0;
                }
                vertex_number = current->child[work_string[suffix + distance]];
                current = &vertices[vertex_number];
            }

            // invariant of cycle: len + distance = phase  - suffix
            if(len + distance != phase - suffix) {
                throw 2;
            }
            result_stack.emplace_back(vertex_number);
            // internal vertex handle
            if(len == current->len()) {
                if(previous_internal != 0 && suffix == internal) {
                    vertices[previous_internal].link = vertex_number;
                    previous_internal = 0;
                }
                if(current->child.find(c) == current->child.end()) {
                    current->child[c] = vertices_count;
                    vertices.emplace_back(vertex(phase, work_string.size(), vertex_number, 0));
                    ++vertices_count;
                    suffix_link = vertices[vertex_number].link;
                    if(distance < 0) {
                        throw 3;
                    }
                }
                else {
                    return suffix;
                }
            }
            else {
                // experimental split
                if(work_string[current->left + len] != c) {
                    if(previous_internal != 0 && suffix == internal) {
                        vertices[previous_internal].link = vertices_count;
                    }
                    previous_internal = vertices_count;
                    internal = suffix + 1;

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
                }
                else {
                    return suffix;
                }
            }
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
    //s = "aaab$";
    //t = "abaabaaabaaaab#";
    //cin >> s >> t;
    string concat = s + t;
    SuffixTree mysuf(concat, true);
    //SuffixTree badsuf(concat, false);
    mysuf.build_tree();
    //badsuf.build_tree();
    //badsuf.compare(mysuf);
    size_t number = 0;
    cout << mysuf.get_size() << endl;
    mysuf.dfs(0, number, s.size(), t.size(), 0);
    //mysuf.show_internal_links();
    return 0;
}