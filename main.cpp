#include <bits/stdc++.h>

using namespace std;

class Bohr {
    // (vertex, char)[coded into 1 int] -> (son of vertex by char)
    unordered_map <int, short int> edges;
    // (parent of vertex, vertex)[coded into 1 int] -> (char of edge between them)
    unordered_map <int, char> board;
    // indices of parents of vertices
    vector <int> par;
    // indices of vertices
    vector <vector <int> > ind;
    // 'index' - index of the next terminal state, 'lind' - index of the next vertex to be added
    int index, lind; 

    //adding string to trie
    void add_str(string &s) {
        if(!s.length())
            return;
        int i = 0;
        for(char c : s) {
            int cod = code(i, c);
            if(edges.find(cod) == edges.end()) {
                edges[cod] = lind;
                vector <int> k;
                ind.emplace_back(k);
                par.emplace_back(i);
                board[code_int(i, lind)] = c;
                i = lind++;
            }
            else
                i = edges[cod];
        }
        ind[i].emplace_back(index++);
    }

public:
    Bohr(vector <string> &mstr) {
        index = 0;
        lind = 1;
        // zero index automatically goes to root
        ind.resize(1);
        // the root is a parent of himself
        par.resize(1, 0);
        for(string s : mstr)
            add_str(s);
    }

    //getters
    int get_ind_size(int i) {
        return ind[i].size();
    }

    vector <int>& get_ind(int i) {
        return ind[i];
    }

    int get_par(int i) {
        return par[i];
    }

    int get_edge(int i, char c) {
        int cod =  code(i, c);
        return edges.find(cod) == edges.end() ? -1 : edges[cod];
    }

    char get_board(int i, int j) {
        return board[code_int(i, j)];
    }

    int get_size() {
        return ind.size();
    }
    // coders
    int code_int(int i, int j) const {
        return (i << 15) + j;
    }

    int code(int i, char c) const {
        return (i << 15) + static_cast<unsigned int>(c);
    }
};

// approved by all approved parts
class Automate {
    // u - suffix links, t - terminal suffix links
    vector <int> u, t;
    // d - transitional function
    unordered_map <int, int> d;
    Bohr *ref;
    // [current] state [of the automate] - number of some vertex
    int state = 0;
    // it compresses the results given by terminal[suffix link] function
    vector <vector <int> > super;

    int suffix_link(int num) {
        if(u[num] != -1)
            return u[num];
        int parent = ref->get_par(num);
        if(parent == 0)
            return u[num] = 0;
        return u[num] = func(suffix_link(parent), ref->get_board(parent, num));
    }

    int terminal(int num) {
        if(t[num] != -1)
            return t[num];
        if(ref->get_ind_size(suffix_link(num)))
            t[num] = suffix_link(num);
        else if(suffix_link(num) == 0)
            t[num] = 0;
        else
            t[num] = terminal(suffix_link(num));
        return t[num];
    }

    int func(int num, char c) {
        int cod = ref->code(num, c);
        if(d.find(cod) != d.end())
            return d[cod];
        if(ref->get_edge(num, c) != -1)
            d[cod] = ref->get_edge(num, c);
        else if(num == 0)
            d[cod] = 0;
        else
            d[cod] = func(suffix_link(num), c);
        return d[cod];
    }

public:

    Automate(Bohr& trie) {
        ref = &trie;
        u.resize(ref->get_size(), -1);
        t.resize(ref->get_size(), -1);
        super.resize(ref->get_size());
        u[0] = 0;
    }

    // в русском языке автомат стреляет, поэтому такое название
    vector <int> fire(char c) {
        int cur = func(state, c);
        state = cur;
        // if super[state] is not empty - then result was received previously
        // so we don't need to calculate it again
        if(super[state].size())
            return super[state];
        vector <int> list;
        // jumping over terminal links
        // first, of course may be not a terminal one, but size of list'll be empty
        while(cur) {
            for(int i : ref->get_ind(cur))
                list.emplace_back(i);
            cur = terminal(cur);
        }
        return super[state] = list;
    }

};

int main() {
    // speed-up
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    string s;
    getline(cin, s);
    //каретка головного мозга
    if(s[s.length() - 1] == '\r')
        s.erase(s.end() - 1);

    vector <string> mstr;
    vector <int> ind;
    int cur = -1;
    // splits text into a words by '?'
    for(int i = 0; i < s.length();) {
        // new string for trie
        if(s[i] != '?') {
            cur++;
            mstr.emplace_back("");
            mstr[cur] += s[i++];
            while(s.length() > i && s[i] != '?')
                mstr[cur] += s[i++];
            ind.emplace_back(i - 1);
        }

        while(i < s.length() && s[i] == '?')
            i++;
    }

    int pos = 0;

    Bohr bor(mstr);
    Automate mate(bor);
    if(mstr.size() == 0) {
        for(int i = 0; i < pos - s.length() + 1; i++)
            cout << i << " ";
        return 0;
    }
    mstr.clear();

    char c;
    vector <short int> mappy;
    // determines positions of pattern in the text
    while(cin >> c) {
        vector <int> list = mate.fire(c);
        mappy.emplace_back(0);
        for(int i : list) {
            int position = pos - ind[i];
            if(position < 0)
                continue;
            mappy[position]++;
        }
        pos++;
    }

    // handles the situation when there are '?' in the end of string
    for(int i = 0; i < mappy.size(); i++) {
        if(mappy[i] == cur + 1 && i + s.length() <= pos)
            cout << i << " ";
    }
    return 0;
}