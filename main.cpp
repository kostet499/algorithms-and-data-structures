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
    point() : x(std::nan("")), y(std::nan("")) {
    }
    static double Angle(const point &a, const point &b);
    static bool IsCounter(const point &a, const point &b, const point &c);
};

double point::Angle(const point &a, const point &b) {
    double one = a.x * b.x + a.y * b.y;
    double two = a.x * b.y - a.y * b.x;
    return atan2(two, one);
}

bool point::IsCounter(const point &a, const point &b, const point &c) {
    return Angle(b - a, c - a) > 0;
}


struct line {
    static double comparing_precision;
    static double endless;
    point first;
    point second;

    line(point p1, point p2) : first(p1), second(p2) {
    }

    double Tilt() const {
        return (first.y - second.y) / (first.x - second.x);
    }

    double Intercept() const {
        return first.y - Tilt() * first.x;
    }

    bool IsVertical() const {
        return IsZero(first.x - second.x);
    }

    bool IsHorizontal() const {
        return IsZero(first.y - second.y);
    }

    point Middle() const {
        return (first + second) * 0.5;
    }

    // duck this sheet
    // line computations
    // no duckin'-cluckin' nans
    static point Intersect(const line &a, const line &b) {
        if((a.IsHorizontal() && b.IsHorizontal()) || (a.IsVertical() && b.IsVertical())) {
            return {};
        }
        // ap tries to point to horizontal, bp - to vertical
        const line *ap = &a;
        const line *bp = &b;
        if(b.IsHorizontal() || a.IsVertical()) {
            std::swap(ap, bp);
        }

        if(ap->IsHorizontal() && bp->IsVertical()) {
            return {bp->first.x, ap->first.y};
        }

        if(ap->IsHorizontal()) {
            return {(ap->first.y - bp->Intercept()) / bp->Tilt(), ap->first.y};
        }

        if(bp->IsVertical()) {
            return {bp->first.x, bp->first.x * ap->Tilt() + ap->Intercept()};
        }

        double x = (bp->Intercept() - ap->Intercept()) / (ap->Tilt() - bp->Tilt());
        return {x, x * ap->Tilt() + ap->Intercept()};
    }

    bool IsOnLine(const point &dot) const {
        if(std::isnan(dot.x) || std::isnan(dot.y)) {
            return false;
        }

        std::pair<double, double> x_coord = std::make_pair(std::min(first.x, second.x) - comparing_precision, std::max(first.x, second.x) + comparing_precision);
        std::pair<double, double> y_coord = std::make_pair(std::min(first.y, second.y) - comparing_precision, std::max(first.y, second.y) + comparing_precision);

        return dot.x > x_coord.first && dot.x < x_coord.second && dot.y > y_coord.first && dot.y < y_coord.second;
    }

    bool HasInfinitPoint() const {
        return
        std::min(fabs(fabs(first.x) - endless), fabs(fabs(first.y) - endless)) < 1e1 ||
        std::min(fabs(fabs(second.x) - endless), fabs(fabs(second.y) - endless)) < 1e1;
    }

    // делит линию по точке ray_start так, чтобы на ней остался отрезок [ray_start, ray_point]
    line Split(const point &ray_start, const point &ray_point) const {
        line left = line(ray_start, first);
        line right = line(ray_start, second);
        if(IsOnLine(ray_point)) {
            return left;
        }
        else {
            return right;
        }
    }

    static bool IsZero(double value) {
        return fabs(value) < comparing_precision;
    }

    line Sym() const {
        return {second, first};
    }

    // k1 * k2 = -1
    // "endless" bisector line
    line BisectorLine() const {
        point middle = Middle();
        if(IsVertical()) {
            // перендикуляр горизонтальный
            point bis1(-endless, middle.y);
            point bis2(endless, middle.y);
            return {bis1, bis2};
        }

        if(IsHorizontal()) {
            // перпендикуляр вертикальный
            point bis1(middle.x, -endless);
            point bis2(middle.x, endless);
            return {bis1, bis2};
        }

        double tilt = -1.0 / Tilt();
        double intercept = middle.y - middle.x * tilt;
        return {point(-endless, -endless * tilt + intercept), point(endless, endless * tilt + intercept)};
    }
};

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
            while(curr > 1 && !point::IsCounter(sorted_list[hull[curr - 2]], sorted_list[hull[curr - 1]], sorted_list[i])) {
                --curr;
            }
            hull[curr] = i;
            ++curr;
        }

        size_t optimal_size = curr;
        // the last point will be already in the down list
        for(size_t i = end - 2;  i > - 1; --i) {
            while(curr > optimal_size && !point::IsCounter(sorted_list[hull[curr - 2]], sorted_list[hull[curr - 1]], sorted_list[i])) {
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

class VoronoiDiargam {
public:
    // end - excluding
    explicit VoronoiDiargam(const std::vector<point> &points, size_t begin, size_t end) : point_set(points){
        start = begin;
        finish = end;
        if(end - begin == 2) {
            Build2(begin, end);
            return;
        }
        if(end - begin == 3) {
            Build3(begin, end);
            return;
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
                trulik &= para.HasInfinitPoint();
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

    line LowestCommonSupport(const ConvexAndrew& voron, const ConvexAndrew &eagle) const {

    }

    line UpperCommonSupport(const ConvexAndrew& voron, const ConvexAndrew &eagle) const {

    }

    void Build2(size_t begin, size_t end) {
        double comparing_precision = 1e-10;
        polygons.resize(2);
        line temp = line(point_set[begin], point_set[begin + 1]);
        line bisector = temp.BisectorLine();
        if(point::IsCounter(temp.first, temp.second, bisector.second)) {
            polygons[0].emplace_back(bisector);
            polygons[1].emplace_back(bisector.Sym());
        }
        else {
            polygons[0].emplace_back(bisector.Sym());
            polygons[1].emplace_back(bisector);
        }
    }

    // центр описанной около треугольника окружности
    // пересечение двух серединных перпендикуляров
    void Build3(size_t begin, size_t end) {
        polygons.resize(3);
        line ab(point_set[begin], point_set[begin + 1]);
        line bc(point_set[begin + 1], point_set[begin + 2]);
        line ca(point_set[begin + 2], point_set[begin]);

        line bis1 = ab.BisectorLine();
        line bis2 = bc.BisectorLine();
        line bis3 = ca.BisectorLine();

        // это же и есть вершина диаграмы
        point circle_center = line::Intersect(bis1, bis2);

        // теперь нужно построить три луча от центра окружности
        line ray1 = bis1.Split(circle_center, ab.Middle());
        line ray2 = bis2.Split(circle_center, bc.Middle());
        line ray3 = bis3.Split(circle_center, ca.Middle());

        // теперь нужно правильно вставить два луча в каждый полигон - против часовой стрелки
        // так как точки отсорчены по x, вообще говоря есть два случая: вторая точка над прямой первой-третьей или нет
        if(point::IsCounter(point_set[begin], point_set[begin + 2], point_set[begin + 1])) {
            polygons[0].emplace_back(ray3.Sym());
            polygons[0].emplace_back(ray1);
            polygons[1].emplace_back(ray1.Sym());
            polygons[1].emplace_back(ray2);
            polygons[2].emplace_back(ray2.Sym());
            polygons[2].emplace_back(ray3);
        }
        else {
            polygons[0].emplace_back(ray1.Sym());
            polygons[0].emplace_back(ray3);
            polygons[1].emplace_back(ray2.Sym());
            polygons[1].emplace_back(ray1);
            polygons[2].emplace_back(ray3.Sym());
            polygons[2].emplace_back(ray2);
        }
    }
private:
    // храним диаграму для каждого сайта
    // многоугольник ребер Вороного с обходом против часовой стрелки
    // для каждой вершины связно по индексации храним является ли она точкой или лишь направляющей бесконечного луча
    // во время обхода возникнет ребро между такими мнимыми точками, как то захэндлим
    size_t start;
    size_t finish;

    // можем нумеровать сайты в естественном порядке (по сортировке по x-коориднате, чтобы связать с нумерацией в массиве)
    std::vector<std::list<line> > polygons;
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

double line::comparing_precision = 1e-10;
double line::endless = 1e10;
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