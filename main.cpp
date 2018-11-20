#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>
#include <list>

/*
 * http://www.personal.kent.edu/~rmuhamma/Compgeometry/MyCG/Voronoi/DivConqVor/divConqVor.htm
 * https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=4567872&tag=1
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
    point operator*(double number) const {
        return {this->x * number, this->y * number};
    }
    bool operator<(const point &b) const {
        return (this->x < b.x) || (this->x == b.x && this->y < b.y);
    }
    double operator()() const {
        return std::pow(this->x, 2) + std::pow(this->y, 2);
    }
    point() : x(0.0 / 0), y(0.0 / 0) {
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
            while(curr > 1 && !is_counter(sorted_list[hull[curr - 2]], sorted_list[hull[curr - 1]], sorted_list[i])) {
                --curr;
            }
            hull[curr] = i;
            ++curr;
        }

        size_t optimal_size = curr;
        // the last point will be already in the down list
        for(size_t i = end - 2;  i > - 1; --i) {
            while(curr > optimal_size && !is_counter(sorted_list[hull[curr - 2]], sorted_list[hull[curr - 1]], sorted_list[i])) {
                --curr;
            }
            hull[curr] = i;
            ++curr;
        }
        // hull is cycled
        hull.resize(curr);
    }

    const size_t &operator[](size_t index) {
        return hull[index];
    }

    size_t size() {
        return hull.size() - 1;
    }

private:
    std::vector<size_t> hull;
};

using line = std::pair<point, point>;
class VoronoiDiargam {
public:
    // end - excluding
    explicit VoronoiDiargam(const std::vector<point> &points, size_t begin, size_t end) : point_set(points){
        start = begin;
        finish = end;
        if(end - begin == 2) {
            build2(begin, end);
        }
        else if(end - begin == 3) {
            build3(begin, end);
        }
        size_t split_key = (begin + end) / 2;
        VoronoiDiargam voron(point_set, begin, split_key);
        VoronoiDiargam eagle(point_set, split_key, end);
        Merge(*this, voron, eagle, begin, end);
    }

    size_t Average() {
        size_t limited_edges = 0;
        size_t limited_polygons = 0;
        for(auto &polygon : polygons) {
            bool trulik = true;
            for(auto &para : polygon) {
                trulik &= para.second;
            }
            if(trulik) {
                ++limited_polygons;
                // because each polygon is cycled we get -1
                limited_edges += polygon.size();
            }
        }
        return limited_edges / limited_polygons;
    }
private:
    VoronoiDiargam Merge(const VoronoiDiargam &result, const VoronoiDiargam &voron, const VoronoiDiargam &eagle,
            size_t begin, size_t end) {
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

    // duck this sheet
    // line computations
    // nans can then transfer to angles if dies
    point Intersect(const line &a, const line &b) {
        // coefs determination
        double coefa = (a.first.y - a.second.y) / (a.first.x - a.second.x);
        double coefb = (b.first.y - b.second.y) / (b.first.x - b.second.x);
        if((is_zero(coefa) && is_zero(coefb)) || (std::isnan(coefa) && std::isnan(coefb))) {
            return {};
        }
        // ap tries to point to horizontal, bp - to vertical
        const line *ap = &a;
        const line *bp = &b;
        if(is_zero(coefb) || std::isnan(coefa)) {
            std::swap(coefa, coefb);
            std::swap(ap, bp);
        }

        if(is_zero(coefa) && std::isnan(coefb)) {
            return {bp->first.x, ap->first.y};
        }

        // intercept
        double intera = ap->first.y - coefa * ap->first.x;
        double interb = bp->first.y - coefb * bp->first.x;
        if(is_zero(coefa)) {
            return {(ap->first.y - interb) / coefb, ap->first.y};
        }

        if(std::isnan(coefb)) {
            return {bp->first.x, bp->first.x * coefa + intera};
        }

        double x = (interb - intera) / (coefa - coefb);
        return {x, x * coefa + intera};
    }

    bool isOnLine(const point &dot, const line &b) {
        if(std::isnan(dot.x) || std::isnan(dot.y)) {
            return false;
        }

        double comparing_precision = 1e-10;
        std::pair<double, double> x_coord = std::make_pair(std::min(b.first.x, b.second.x) - comparing_precision, std::max(b.first.x, b.second.x) + comparing_precision);
        std::pair<double, double> y_coord = std::make_pair(std::min(b.first.x, b.second.x) - comparing_precision, std::max(b.first.x, b.second.x) + comparing_precision);

        return dot.x > x_coord.first && dot.x < x_coord.second && dot.y > y_coord.first && dot.y < y_coord.second;
    }

    // k1 * k2 = -1
    line BisectorLine(const line &a) {

    }

    bool is_zero(double value) {
        return fabs(value) < 1e-10;
    }

    void build2(size_t begin, size_t end) {
        double comparing_precision = 1e-10;
        polygons.resize(2);
        line temp = line(point_set[begin], point_set[begin + 1]);
        line bis = BisectorLine(temp);
        if(is_counter(temp.first, temp.second, bis.second)) {
            polygons[0].emplace_back(make_pair(line(bis.first, bis.second), false));
            polygons[1].emplace_back(make_pair(line(bis.second, bis.first), false));
        }
        else {
            polygons[1].emplace_back(make_pair(line(bis.first, bis.second), false));
            polygons[0].emplace_back(make_pair(line(bis.second, bis.first), false));
        }
    }

    // центр описанной около треугольника окружности
    // пересечение двух серединных перпендикуляров
    void build3(size_t begin, size_t end) {
        polygons.resize(3);
        point a = point_set[begin];
        point b = point_set[begin + 1];
        point c = point_set[begin + 2];

    }
private:
    // храним диаграму для каждого сайта
    // многоугольник ребер Вороного с обходом против часовой стрелки
    // для каждой вершины связно по индексации храним является ли она точкой или лишь направляющей бесконечного луча
    // во время обхода возникнет ребро между такими мнимыми точками, как то захэндлим
    size_t start;
    size_t finish;

    // можем нумеровать сайты в естественном порядке (по сортировке по x-коориднате, чтобы связать с нумерацией в массиве)
    std::vector<std::vector<std::pair<line, bool> > > polygons;
    const std::vector<point> &point_set;
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