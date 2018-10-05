#include <iostream>
#include <vector>

using std::vector;
using std::string;
using std::cout;
using std::endl;

class SuffixArray {
public:
    SuffixArray(string &);

    vector<int>* get_sorted_suffix() {
        return &sorted_suffix;
    }

    // least common prefix
    int lcp(int, int);

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

SuffixArray::SuffixArray(string &build_string) {
    symbol_to_fill = '#';
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

    return 0;
}