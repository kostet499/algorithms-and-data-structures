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
public:
    explicit SparseTable(const vector<size_t > &build_vector) {
        initialise(build_vector);
    }

    SparseTable() = default;

    size_t get_min(size_t left, size_t right) const {
        size_t j = log(right - left + 1);
        size_t next = right + 1 - (1 << j);
        return min(table[next][j], table[left][j]);
    }
private:
    size_t len = 0, wid = 0;
    vector<vector<size_t > > table;
private:
    void initialise(const vector <size_t > &a) {
        len = a.size(), wid = log(len) + 1;
        table.resize(len);
        for(size_t i = 0; i < len; ++i) {
            table[i].resize(wid, INT32_MAX);
            table[i][0] = a[i];
        }

        for(size_t j = 1; j < wid; ++j) {
            for(size_t i = 0; i < len; ++i) {
                size_t next = i + (1 << (j - 1));
                size_t fst = table[i][j - 1];
                size_t scd = INT32_MAX;
                if(next < len) {
                    scd = table[next][j - 1];
                }
                table[i][j] = min(fst, scd);
            }
        }
    }

    size_t log(size_t len) const {
        if(len == 1)
            return 0;
        return log(len >> 1) + 1;
    }
};

class SuffixArray {
public:
    explicit SuffixArray(const string &);

    explicit SuffixArray(const string &, char);

    const vector<size_t> &get_sorted_suffix() const {
        return sorted_suffix;
    }

    const vector<size_t> &get_lcp() const {
        return lcp_values;
    }

    size_t real_size() const {
        return work_string.size();
    }

    size_t &operator[](size_t i) {
        return sorted_suffix[i];
    }

    size_t lcp(size_t a, size_t b) {
        a = classes_equal[a];
        b = classes_equal[b];
        if(a > b)
            std::swap(a, b);
        return table.get_min(a, b - 1);
    }
private:
    vector<size_t> sorted_suffix;
    vector<size_t> classes_equal;
    vector<size_t> lcp_values;
    string work_string;
    size_t char_size;
    size_t classes_number;
    char symbol_to_fill;
    SparseTable table;
private:
    void make_sorted_first();

    void make_sorted_finish();

    void sort_group(int);

    void build_lcp();

    void make_work_string(const string &);

    int index_char(char) const;

    int step_left(int index, int step) const;

    int step_right(int index, int step) const;
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
    size_t current_length = 0;
    for(size_t i = 0; i < lcp_values.size(); ++i) {
        if(current_length > 0) {
            --current_length;
        }

        if(classes_equal[i] == lcp_values.size() - 1) {
            lcp_values[lcp_values.size() - 1] = 0;
            current_length = 0;
        }
        else {
            size_t prev = sorted_suffix[classes_equal[i] + 1];
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

SuffixArray::SuffixArray(const string &build_string) : SuffixArray(build_string, '#') {
}

SuffixArray::SuffixArray(const string &build_string, char c) : classes_number(0), symbol_to_fill(c), char_size(256) {
    make_work_string(build_string);
    // 256 classes because of 1-byte char
    classes_equal.resize(work_string.length());
    sorted_suffix.resize(work_string.length());
    lcp_values.resize(work_string.length());
    make_sorted_first();
    make_sorted_finish();
    build_lcp();
    table = SparseTable(lcp_values);
}

void SuffixArray::make_work_string(const string &build_string) {
    work_string = build_string;
}

void SuffixArray::make_sorted_first() {
    vector<int> classes_symbol(char_size);

    // we even don't need to sort symbols, encoding does it for us
    for(auto c : work_string) {
        ++classes_symbol[index_char(c)];
    }

    for(size_t i = 1; i < char_size; ++i) {
        classes_symbol[i] += classes_symbol[i - 1];
    }

    for(size_t i = 0; i < work_string.size(); ++i) {
        sorted_suffix[--classes_symbol[index_char(work_string[i])]] = i;
    }

    // giving initial classes to the positions
    size_t current_class = 0;
    classes_equal[0] = current_class;
    for(int i = 1; i < sorted_suffix.size(); ++i) {
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
    vector<size_t> group(sorted_suffix.size());

    vector<size_t> classes(classes_number);
    for(auto j : classes_equal) {
        ++classes[j];
    }

    for(size_t i = 1; i < classes_number; ++i) {
        classes[i] += classes[i - 1];
    }

    for(int i = static_cast<int>(sorted_suffix.size()) - 1; i > - 1; --i) {
        int prev = step_left(static_cast<int>(sorted_suffix[i]), step);
        group[--classes[classes_equal[prev]]] = static_cast<size_t>(prev);
    }

    vector<size_t> new_classes_equal(sorted_suffix.size());
    size_t new_classes_number = 1;
    size_t old_class1 = classes_equal[group[0]];
    size_t old_class2 = classes_equal[step_right(static_cast<int>(group[0]), step)];
    for(int i = 1; i < group.size(); ++i) {
        size_t class1 = classes_equal[group[i]];
        size_t class2 = classes_equal[step_right(static_cast<int>(group[i]), step)];
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
    std::ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    string a;
    string b;
    long long k = 0;

    cin >> a >> b >> k;

    string concat = a + '?' + b + "#";
    SuffixArray workflow(concat, '#');

    size_t a_end_size = a.length() + 1;
    size_t b_end_size = workflow.real_size() - a.length() - 1;

    vector <size_t> a_suf(a_end_size);
    vector <size_t> b_suf(b_end_size);
    size_t index_a = 0;
    size_t index_b = 0;
    for(size_t i = 0; i < workflow.real_size(); ++i) {
        size_t current = workflow[i];
        if(current < a_end_size) {
            a_suf[index_a] = i;
            ++index_a;
        }
        else {
            b_suf[index_b] = i;
            ++index_b;
        }
    }

    size_t a_indicator = 0;
    size_t b_indicator = 0;
    long long lexical_position = 0;
    size_t previous = 0;

    while(a_indicator < a_end_size && b_indicator < b_end_size) {
        size_t sufa = workflow[a_suf[a_indicator]];
        size_t sufb = workflow[b_suf[b_indicator]];
        size_t equal_part = workflow.lcp(sufa, sufb);

        if(equal_part > previous) {
            unsigned long long new_lex_pos = lexical_position + equal_part - previous;
            long long new_end = k - lexical_position;
            if(new_lex_pos >= k) {
                for(size_t i = 0; i < previous + new_end; ++i) {
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