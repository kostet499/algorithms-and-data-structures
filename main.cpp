#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>

/*
 * http://www.personal.kent.edu/~rmuhamma/Compgeometry/MyCG/Voronoi/DivConqVor/divConqVor.htm
 */

struct point {
    double x;
    double y;

    point(double a1, double a2) : x(a1), y(a2){
    }
    point operator+(const point& b) {
        return {this->x + b.x, this->y + b.y};
    }
    point operator-(const point& b) {
        return {this->x - b.x, this->y - b.y};
    }
    bool operator<(const point &b) {
        return (this->x < b.x) || (this->x == b.x && this->y < b.y);
    }
    double operator()() {
        return std::pow(this->x, 2) + std::pow(this->y, 2);
    }
    point() : x(0), y(0) {
    }
};

// точки a, b, c формируют треугольник с обходом против часовой стрелки
bool CCW(point a, point b, point c) {
    return a.x * (b.y - c.y) - b.x * (a.y - c.y) + c.x * (a.y - b.y) > 0;
}

// мотивирован ими - реализовать самому не сложно по описанию
// но они очень красиво оптимизировали сам алгоритм (чтобы кучи перекидок буфферов не было), так что просто слегка подсматривал
// http://www.algorithmist.com/index.php/Monotone_Chain_Convex_Hull
class ConvexAndrew{
public:
    // end - excluding
    explicit ConvexAndrew(const std::vector<point> &sorted_list, size_t begin, size_t end) {
        hull.resize(2 * (end - begin));
        size_t curr = 0;
        for(size_t i = begin; i < end; ++i) {
            while(curr > 1 && !CCW(hull[curr - 2], hull[curr - 1], sorted_list[i])) {
                --curr;
            }
            hull[curr] = sorted_list[i];
            ++curr;
        }

        size_t optimal_size = curr;
        // the last point will be already in the down list
        for(size_t i = end - 2;  i > - 1; --i) {
            while(curr > optimal_size && !CCW(hull[curr - 2], hull[curr - 1], sorted_list[i])) {
                --curr;
            }
            hull[curr] = sorted_list[i];
            ++curr;
        }
        // hull is cycled
        hull.resize(curr);
    }

    const point &operator[](size_t index) {
        return hull[index];
    }

    size_t size() {
        return hull.size() - 1;
    }

private:
    std::vector<point> hull;
};

// в принципе в случае 4 точек на одной окружности должен вообще стать нулём)
bool InCircle(point a, point b, point c, point d) {
    // считаем суперопределитель
    double part1 = a.x * (b.y * (c() - d()) - c.y * (b() - d()) + d.y * (b() - c()) );
    double part2 = b.x * (a.y * (c() - d()) - c.y * (a() - d()) + d.y * (a() - c()) );
    double part3 = c.x * (a.y * (b() - d()) - b.y * (a() - d()) + d.y * (a() - b()) );
    double part4 = d.x * (a.y * (b() - c()) - b.y * (a() - c()) + c.y * (a() - b()) );
    return part1 - part2 + part3 - part4 > 0;
}

using line = std::pair<point, point>;
class VoronoiDiargam {
public:
    // end - excluding
    explicit VoronoiDiargam(const std::vector<point> &point_set, size_t begin, size_t end) {
        size_t split_key = (begin + end) / 2;
        VoronoiDiargam voron(point_set, begin, split_key);
        VoronoiDiargam eagle(point_set, split_key, end);
        Merge(*this, voron, eagle, point_set, begin, end);
    }
private:
    VoronoiDiargam Merge(const VoronoiDiargam &result, const VoronoiDiargam &voron, const VoronoiDiargam &eagle,
                         const std::vector<point> &point_set, size_t begin, size_t end) {
        size_t split_key = (begin + end) / 2;
        // step 1
        ConvexAndrew convex_voron(point_set, begin, split_key);
        ConvexAndrew convex_eagle(point_set, split_key, end);

        // step 2

    }

    line LowestCommonSupport(const ConvexAndrew& voron, const ConvexAndrew &eagle) {

    }
private:
};

void prepare_data(std::vector<point> &data) {
    double x;
    double y;
    while(std::cin >> x) {
        std::cin >> y;
        data.emplace_back(point(x, y));
    }
    // sorting by x-coord for Divide&Conquer + Andrew's monotone chain
    sort(data.begin(), data.end());
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::vector<point> dots;
    prepare_data(dots);
    // all sites are unbounded
    if(dots.size() < 4) {
        std::cout << 0;
    }
    VoronoiDiargam voron(dots, 0, dots.size());
    return 0;
}