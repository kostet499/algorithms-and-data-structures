#include <bits/stdc++.h>

using namespace std;

class Bohr {
    map <int, short int> edges;
    map <int, char> board;
    // массив индексов бора и массив предков индексов
    vector <int> par;
    vector <vector <int> > ind;
    // индекс для пометок терминальных состояний, индекс для пометок новых веток
    int index, lind;

    //добавляем строку в бор
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
        ind.resize(1);
        par.resize(1, 0);
        for(string s : mstr)
            add_str(s);
    }

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

    int code_int(int i, int j) const {
        return (i << 15) + j;
    }

    int code(int i, char c) const {
        return (i << 15) + static_cast<unsigned int>(c);
    }
};

// approved by all approved parts
class Automate {
    vector <int> u, t;
    unordered_map <int, int> d;
    Bohr *ref;
    int state = 0;
    vector <vector <int> > super;

    //approved
    int suffix_link(int num) {
        if(u[num] != -1)
            return u[num];
        int parent = ref->get_par(num);
        if(parent == 0)
            return u[num] = 0;
        return u[num] = func(suffix_link(parent), ref->get_board(parent, num));
    }

    //approved
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

    //approved
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
    //approved
    Automate(Bohr& trie) {
        ref = &trie;
        u.resize(ref->get_size(), -1);
        t.resize(ref->get_size(), -1);
        super.resize(ref->get_size());
        u[0] = 0;
    }

    //approved
    vector <int> fire(char c) {
        int cur = func(state, c);
        state = cur;
        if(super[state].size())
            return super[state];
        vector <int> list;
        while(cur) {
            for(int i : ref->get_ind(cur))
                list.emplace_back(i);
            cur = terminal(cur);
        }
        return super[state] = list;
    }

};

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    string s;
    getline(cin, s);
    if(s[s.length() - 1] == '\r')
        s.erase(s.end() - 1);
    vector <string> mstr;
    vector <int> ind;
    int cur = -1;
    for(int i = 0; i < s.length();) {
        // новая строка-шаблон
        if(s[i] != '?') {
            cur++;
            mstr.emplace_back("");
            mstr[cur] += s[i++];
            while(s.length() > i && s[i] != '?')
                mstr[cur] += s[i++];
            ind.emplace_back(i - 1);
        }
        // пропускаем все вопросики и фактически либо заканчиваем слово, либо с следущей итерации начнем новое
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

    for(int i = 0; i < mappy.size(); i++) {
        if(mappy[i] == cur + 1 && i + s.length() <= pos)
            cout << i << " ";
    }
    return 0;
}