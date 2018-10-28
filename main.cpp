#include <iostream>
#include <vector>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::cin;
using std::pair;
using std::make_pair;
using std::min;
using std::max;

class SparseTable {
    int len = 0, wid = 0;
    vector <vector<int> > table;
    void initialise(vector <int>& a) {
        len = a.size(), wid = log(len) + 1;
        table.resize(len);
        for(int i = 0; i < len; i++) {
            table[i].resize(wid, INT32_MAX);
            table[i][0] = a[i];
        }

        for(int j = 1; j < wid; j++) {
            for(int i = 0; i < len; i++) {
                int next = i + (1 << (j - 1));
                int fst = table[i][j - 1];
                int scd = INT32_MAX;
                if(next < len) {
                    scd = table[next][j - 1];
                }
                table[i][j] = min(fst, scd);
            }
        }
    }

    int log(int len) {
        if(len == 1)
            return 0;
        return log(len / 2) + 1;
    }

public:
    explicit SparseTable(vector <int>& build_vector) {
        initialise(build_vector);
    }

    SparseTable() = default;

    int get_min(int left, int right) {
        int j = log(right - left + 1);
        int next = right + 1 - (1 << j);
        return min(table[next][j], table[left][j]);
    }
};


class SuffixArray {
public:
    SuffixArray(string &, char);

    const vector<int> &get_sorted_suffix() {
        return sorted_suffix;
    }

    const vector<int> &get_lcp() {
        return lcp_values;
    }

    size_t real_size() {
        return work_string.size();
    }

    int &operator[](int i) {
        return sorted_suffix[i];
    }

    int lcp(int a, int b) {
        a = classes_equal[a];
        b = classes_equal[b];
        if(a > b)
            std::swap(a, b);
        return table.get_min(a, b - 1);
    }

private:
    vector<int> sorted_suffix;
    vector<int> classes_equal;
    vector<int> lcp_values;
    string work_string;
    int char_size;
    int classes_number;
    SparseTable table;

    char symbol_to_fill;

    void make_sorted_first();

    void make_sorted_finish();

    int index_char(char) const;

    void make_work_string(string &);

    void sort_group(int);

    int step_left(int index, int step) const;

    int step_right(int index, int step) const;

    void build_lcp();
};

int SuffixArray::step_left(int index, int step) const {
    index -= step;
    if(index < 0) {
        index += sorted_suffix.size();
    }
    return index;
}

// classes_equal is suf^(-1) array
void SuffixArray::build_lcp() {
    int current_length = 0;
    for(int i = 0; i < lcp_values.size(); ++i) {
        if(current_length > 0) {
            --current_length;
        }

        if(classes_equal[i] == lcp_values.size() - 1) {
            lcp_values[lcp_values.size() - 1] = 0;
            current_length = 0;
        }
        else {
            int prev = sorted_suffix[classes_equal[i] + 1];
            while(std::max(prev + current_length, i + current_length) < lcp_values.size()
                    && work_string[prev + current_length] == work_string[i + current_length]) {
                ++current_length;
            }
            lcp_values[classes_equal[i]] = current_length;
        }
    }
}

int SuffixArray::step_right(int index, int step) const {
    index += step;
    if(index >= sorted_suffix.size()) {
        index -= sorted_suffix.size();
    }
    return index;
}

SuffixArray::SuffixArray(string &build_string, char symbol) {
    symbol_to_fill = symbol;
    make_work_string(build_string);
    // 256 classes because of 1-byte char
    char_size = 256;
    classes_equal.resize(work_string.length(), 0);
    sorted_suffix.resize(work_string.length(), 0);
    lcp_values.resize(work_string.length(), 0);
    make_sorted_first();
    make_sorted_finish();
    build_lcp();
    table = SparseTable(lcp_values);
}

void SuffixArray::make_work_string(string &build_string) {
    int len = build_string.length();
    int new_len = 1;
    while(new_len < len) {
        new_len <<= 1;
    }
    work_string = build_string;
    for(int i = 0; i < new_len - len; i++) {
        work_string += symbol_to_fill;
    }
}

void SuffixArray::make_sorted_first() {
    vector<int> classes_symbol(char_size, 0);

    // we even don't need to sort symbols, encoding does it for us
    for(auto c : work_string) {
        ++classes_symbol[index_char(c)];
    }

    for(int i = 1; i < char_size; i++) {
        classes_symbol[i] += classes_symbol[i - 1];
    }

    for(int i = 0; i < work_string.size(); i++) {
        sorted_suffix[--classes_symbol[index_char(work_string[i])]] = i;
    }

    // giving initial classes to the positions
    int current_class = 0;
    classes_equal[0] = current_class;
    for(int i = 1; i < sorted_suffix.size(); i++) {
        if(index_char(work_string[sorted_suffix[i]]) == index_char(work_string[sorted_suffix[i - 1]])) {
            classes_equal[sorted_suffix[i]] = current_class;
        }
        else {
            classes_equal[sorted_suffix[i]] = ++current_class;
        }
    }
    classes_number = current_class + 1;
};

void SuffixArray::make_sorted_finish() {
    for(int j = 1; j < work_string.length(); j <<= 1) {
        sort_group(j);
    }
}

void SuffixArray::sort_group(int step) {
    vector<int> group(sorted_suffix.size(), -1);

    vector<int> classes(classes_number, 0);
    for(auto j : classes_equal) {
        ++classes[j];
    }

    for(int i = 1; i < classes_number; i++) {
        classes[i] += classes[i - 1];
    }

    for(int i = sorted_suffix.size() - 1; i > - 1; i--) {
        int prev = step_left(sorted_suffix[i], step);
        group[--classes[classes_equal[prev]]] = prev;
    }

    vector<int> new_classes_equal(sorted_suffix.size(), 0);
    int new_classes_number = 1;
    int old_class1 = classes_equal[group[0]];
    int old_class2 = classes_equal[step_right(group[0], step)];
    for(int i = 1; i < group.size(); i++) {
        int class1 = classes_equal[group[i]];
        int class2 = classes_equal[step_right(group[i], step)];
        if(class1 == old_class1 && class2 == old_class2) {
            new_classes_equal[group[i]] = new_classes_number - 1;
        }
        else {
            new_classes_equal[group[i]] = new_classes_number;
            ++new_classes_number;
        }
        old_class1 = class1;
        old_class2 = class2;
    }
    sorted_suffix = group;
    classes_equal = new_classes_equal;
    classes_number = new_classes_number;
}

// function to give indices to the symbols
int SuffixArray::index_char(char c) const {
    return static_cast<int>(c);
}

int main() {
    std::ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    string a = "";
    string b = "";
    long long k = 0;

    cin >> a;
    cin >> b;
    cin >> k;

    string concat = a + '?' + b + "#";
    SuffixArray conc(concat, '#');

    int a_end_size = a.length() + 1;
    int b_end_size = conc.real_size() - a.length() - 1;

    vector <int> a_suf(a_end_size);
    vector <int> b_suf(b_end_size);
    int index_a = 0;
    int index_b = 0;
    for(int i = 0; i < conc.real_size(); i++) {
        int current = conc[i];
        if(current < a_end_size) {
            a_suf[index_a] = i;
            ++index_a;
        }
        else {
            b_suf[index_b] = i;
            ++index_b;
        }
    }

    int a_indicator = 0;
    int b_indicator = 0;
    long long lexical_position = 0;
    int previous = 0;

    while(a_indicator < a_end_size && b_indicator < b_end_size) {
        int sufa = conc[a_suf[a_indicator]];
        int sufb = conc[b_suf[b_indicator]];
        int equal_part = conc.lcp(sufa, sufb);

        if(equal_part > previous) {
            long long new_lex_pos = lexical_position + equal_part - previous;
            long long new_end = k - lexical_position;
            if(new_lex_pos >= k) {
                for(int i = 0; i < previous + new_end; i++) {
                    cout << a[sufa + i];
                }

                return 0;
            }
            else {
                lexical_position = new_lex_pos;
            }
        }
        previous = equal_part;

        if(a_suf[a_indicator] < b_suf[b_indicator]) {
            ++a_indicator;
        }
        else {
            ++b_indicator;
        }
    }

    cout << -1;
    return 0;
}