//
// Created by robocup on 12.11.18.
//

#ifndef ALGOS_JSONFIELD_H
#define ALGOS_JSONFIELD_H

#include "vector"
#include <math.h>

struct dot {
    double x;
    double y;

    dot(double, double);
    dot();
    double norm();
};

using line = std::pair<dot, dot>;

class JsonField {
public:
    explicit JsonField(const char *filename);
    const std::pair<dot, dot> &operator[](size_t);
    double width() const;
    double height() const;
private:
    double field_width;
    double field_height;
    std::vector<line> field_lines;
};


#endif //ALGOS_JSONFIELD_H
