#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>
#include <list>

/*
 * http://www.personal.kent.edu/~rmuhamma/Compgeometry/MyCG/Voronoi/DivConqVor/divConqVor.htm
 */

struct point {
    double x;
    double y;

    point(double a1, double a2) : x(a1), y(a2){
    }
    point operator+(const point& b) const {
        return {this->x + b.x, this->y + b.y};
    }
    point operator-(const point& b) const {
        return {this->x - b.x, this->y - b.y};
    }
    bool operator<(const point &b) const {
        return (this->x < b.x) || (this->x == b.x && this->y < b.y);
    }
    double operator()() const {
        return std::pow(this->x, 2) + std::pow(this->y, 2);
    }
    point() : x(0), y(0) {
    }
    friend double angle(const point &a, const point &b);
    friend bool is_counter(const point &a, const point &b, const point &c);
};

double angle(const point &a, const point &b) {
    double one = a.x * b.x + a.y * b.y;
    double two = a.x * b.y - a.y * b.x;
    return atan2(two, one);
}

bool is_counter(const point &a, const point &b, const point &c) {
    return angle(b - a, c - a) > 0;
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
            while(curr > 1 && !is_counter(hull[curr - 2], hull[curr - 1], sorted_list[i])) {
                --curr;
            }
            hull[curr] = sorted_list[i];
            ++curr;
        }

        size_t optimal_size = curr;
        // the last point will be already in the down list
        for(size_t i = end - 2;  i > - 1; --i) {
            while(curr > optimal_size && !is_counter(hull[curr - 2], hull[curr - 1], sorted_list[i])) {
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

using line = std::pair<point, point>;
class VoronoiDiargam {
public:
    // end - excluding
    explicit VoronoiDiargam(const std::vector<point> &point_set, size_t begin, size_t end) {
        size_t split_key = (begin + end) / 2;
        start = begin;
        finish = end;
        VoronoiDiargam voron(point_set, begin, split_key);
        VoronoiDiargam eagle(point_set, split_key, end);
        Merge(*this, voron, eagle, point_set, begin, end);
    }

    size_t Average() {
        size_t limited_edges = 0;
        size_t limited_polygons = 0;
        for(auto &polygon : polygons) {
            bool trulik = true;
            size_t sum = 0;
            for(auto &para : polygon) {
                trulik &= para.second;
                ++sum;
            }
            if(trulik) {
                ++limited_polygons;
                // because each polygon is cycled we get -1
                limited_edges += sum - 1;
            }
        }
        return limited_edges / limited_polygons;
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

    line UpperCommonSupport(const ConvexAndrew& voron, const ConvexAndrew &eagle) {

    }
private:
    // храним диаграму для каждого сайта
    // многоугольник вершин Вороного с обходом против часовой стрелки
    // для каждой вершины связно по индексации храним является ли она точкой или лишь направляющей бесконечного луча
    // во время обхода возникнет ребро между такими мнимыми точками, как то захэндлим
    size_t start;
    size_t finish;

    // можем нумеровать сайты в естественном порядке (по сортировке по x-коориднате, чтобы связать с нумерацией в массиве)
    // cycled
    std::vector<std::vector<std::pair<point, bool> > > polygons;
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

    VoronoiDiargam voron(dots, 0, dots.size());
    std::cout << voron.Average();
    return 0;
}