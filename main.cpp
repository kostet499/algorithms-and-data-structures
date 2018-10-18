#include <bits/stdc++.h>
using namespace std;

struct vertex {
    size_t child[28];
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

    int determine(char c) {
        if(c >= 'a') {
            return c - 'a';
        }
        else {
            return 26 + c - '#';
        }
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
        dist_calls = 0;
        in_root = 0;
        previous_vertex_number = 0;
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
            previous = make_phase(i, previous);
        }
    }

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

    size_t get_dist_calls() {
        return dist_calls;
    }

    size_t get_in_root() {
        return in_root;
    }

private:
    string work_string;
    vector<vertex> vertices;
    vector<size_t> result_stack;
    size_t size;
    size_t vertices_count;
    size_t previous_vertex_number;
    bool optimized;
    size_t dist_calls;
    size_t in_root;

    int dist(size_t vertex_number) {
        ++dist_calls;
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
        // distance from root (summing by all edges)
        int distance = 0;
        // symbols that we haven't passed yet
        int len = 0;


        // exp
        size_t internal = 0;

        for(size_t suffix = iterations_begin; suffix <= phase; ++suffix) {
            //suffix_link = 0;
            // we are in the root, so distance from root to root is 0, len is to be counted

            if(!optimized) {
                suffix_link = 0;
            }

            size_t vertex_number = suffix_link;
            vertex *current = &vertices[vertex_number];
            distance = dist(vertex_number);
            len = phase - suffix - distance;
            if(vertex_number == 0) {
                ++in_root;
            }
            while(len > current->len()) {
                len -= current->len();
                distance += current->len();
                vertex_number = current->get_child(work_string[suffix + distance]);
                current = &vertices[vertex_number];
            }


            // result_stack.emplace_back(vertex_number);
            // internal vertex handle
            if(len == current->len()) {
                if(previous_internal != 0 && suffix == internal) {
                    vertices[previous_internal].link = vertex_number;
                    previous_internal = 0;
                }
                if(!current->is_child(c)) {
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
                // experimental split
                if(work_string[current->left + len] != c) {
                    if(previous_internal != 0 && suffix == internal) {
                        vertices[previous_internal].link = vertices_count;
                    }
                    previous_internal = vertices_count;
                    internal = suffix + 1;

                    // new internal vertex with number "vertices_count"
                    vertices[vertex_number].distance = -1;
                    vertices.emplace_back(vertex(current->left, current->left + len, current->parent, 0));
                    current = &vertices[vertex_number];
                    vertices[vertices_count].make_child(work_string[current->left + len], vertex_number);
                    vertices[current->parent].make_child(work_string[current->left], vertices_count);
                    current->parent = vertices_count;
                    current->left += len;
                    ++vertices_count;
                    // new leaf vertex
                    vertices.emplace_back(vertex(phase, work_string.size(), vertices_count - 1, 0));
                    vertices[vertices_count - 1].make_child(c, vertices_count);
                    ++vertices_count;

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

void gen_test(string &s, int size) {
    while(s.size() < size){
        s += "a";
    }
}

int main() {
    cin.tie(0);
    cout.tie(0);
    ios_base::sync_with_stdio(0);
    string s;
    string t;
    //gen_test(s, 100000); s += "$"; gen_test(t, 10000); t += "#";
    cin >> s >> t;
    string concat = s + t;
    SuffixTree mysuf(concat, true);
    //SuffixTree badsuf(concat, false);
    mysuf.build_tree();
    //badsuf.build_tree();
    //badsuf.compare(mysuf);
    size_t number = 0;
    cout << mysuf.get_size() << endl;
    mysuf.dfs(0, number, s.size(), t.size(), 0);
    //cout << mysuf.get_dist_calls() << endl;
    //cout << mysuf.get_in_root() << endl;
    return 0;
}