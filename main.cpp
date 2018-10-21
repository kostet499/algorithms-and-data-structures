#include <iostream>
#include <vector>
#include <utility>
#include <fstream>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::cin;
using std::pair;
using std::make_pair;
using std::min;
using std::max;

class SuffixArray {
public:
    SuffixArray(string &, char);

    int& operator[](int& i) {
        return sorted_suffix[i];
    }

    string& get_work_string() {
        return work_string;
    }

    // least common prefix
    int lcp(int, int);

    int real_size() {
        return sorted_suffix.size();
    }

private:
    vector<int> sorted_suffix;
    vector<vector<int> > classes_equal;
    string work_string;
    int char_size;
    int classes_number;
    int class_index;
    int power2;

    char symbol_to_fill;

    void make_sorted_first();

    void make_sorted_finish();

    int index_char(char) const;

    void make_work_string(string &);

    void sort_group(int);

    int step_left(int, int) const;

    int step_right(int, int) const;
};

int SuffixArray::step_left(int index, int step) const {
    index -= step;
    if(index < 0) {
        index += sorted_suffix.size();
    }
    return index;
}

int SuffixArray::step_right(int index, int step) const {
    index += step;
    if(index >= sorted_suffix.size()) {
        index -= sorted_suffix.size();
    }
    return index;
}

SuffixArray::SuffixArray(string &build_string, char delimiter) {
    symbol_to_fill = delimiter;
    make_work_string(build_string);
    // 256 classes because of 1-byte char
    char_size = 256;
    sorted_suffix.resize(work_string.length(), 0);
    make_sorted_first();
    make_sorted_finish();
}

void SuffixArray::make_work_string(string &build_string) {
    int len = build_string.length();
    int new_len = 1;
    power2 = 0;
    while(new_len < len) {
        new_len <<= 1;
        ++power2;
    }

    work_string = build_string;
    for(int i = 0; i < new_len - len; i++) {
        work_string += symbol_to_fill;
    }

    classes_equal.resize(power2 + 1);
    for(int i = 0; i < power2 + 1; i++) {
        classes_equal[i].resize(new_len, -1);
    }
}

void SuffixArray::make_sorted_first() {
    vector<int> classes_symbol(char_size, 0);
    class_index = 0;
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
    classes_equal[class_index][sorted_suffix[0]] = current_class;
    for(int i = 1; i < sorted_suffix.size(); i++) {
        if(index_char(work_string[sorted_suffix[i]]) == index_char(work_string[sorted_suffix[i - 1]])) {
            classes_equal[class_index][sorted_suffix[i]] = current_class;
        }
        else {
            classes_equal[class_index][sorted_suffix[i]] = ++current_class;
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
    for(auto j : classes_equal[class_index]) {
        ++classes[j];
    }

    for(int i = 1; i < classes_number; i++) {
        classes[i] += classes[i - 1];
    }

    for(int i = sorted_suffix.size() - 1; i > - 1; i--) {
        int prev = step_left(sorted_suffix[i], step);
        group[--classes[classes_equal[class_index][prev]]] = prev;
    }

    int new_classes_number = 1;
    int old_class1 = classes_equal[class_index][group[0]];
    int old_class2 = classes_equal[class_index][step_right(group[0], step)];
    classes_equal[class_index + 1][group[0]] = 0;
    for(int i = 1; i < group.size(); i++) {
        int class1 = classes_equal[class_index][group[i]];
        int class2 = classes_equal[class_index][step_right(group[i], step)];
        if(class1 == old_class1 && class2 == old_class2) {
            classes_equal[class_index + 1][group[i]] = new_classes_number - 1;
        }
        else {
            classes_equal[class_index + 1][group[i]] = new_classes_number;
            ++new_classes_number;
        }
        old_class1 = class1;
        old_class2 = class2;
    }
    sorted_suffix = group;
    ++class_index;
    classes_number = new_classes_number;
}

int SuffixArray::lcp(int fst, int scd) {
    int check_class = power2;
    int answer = 0;
    while(check_class > -1) {
        if(classes_equal[check_class][fst] == classes_equal[check_class][scd]) {
            int step = 1 << check_class;
            answer += step;
            fst = step_right(fst, step);
            scd = step_right(scd, step);
        }
        --check_class;
    }
    return answer;
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

    size_t a_end_size = a.length() + 1;
    size_t b_end_size = conc.real_size() - a.length() - 1;

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