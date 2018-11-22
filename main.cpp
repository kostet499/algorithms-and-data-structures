#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>
#include <list>

/*
 * http://www.personal.kent.edu/~rmuhamma/Compgeometry/MyCG/Voronoi/DivConqVor/divConqVor.htm
 * https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=4567872&tag=1
 * http://sccg.sk/~samuelcik/dgs/quad_edge.pdf
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
    double norm() const {
        return std::pow(this->x, 2) + std::pow(this->y, 2);
    }
    double scalar(const point &b) {
        return this->x * b.x + this->y * b.y;
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
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) > 0;
}

struct line {
    static double comparing_precision;
    point first;
    point second;
    bool fst_endless;
    bool scd_endless;

    bool operator==(const line &b) const {
        return
        this->fst_endless == b.fst_endless &&
        this->scd_endless == b.scd_endless &&
                ((IsZero((b.first - this->first).norm()) && IsZero((b.second - this->second).norm())) ||
                (IsZero((b.first - this->second).norm()) && IsZero((b.second - this->first).norm())));
    }

    double norm() {
        return (second - first).norm();
    }

    line(const point &p1, const point &p2, bool fst, bool scd) : first(p1), second(p2), fst_endless(fst), scd_endless(scd){
    }

    line(const point &p1, const point &p2) : line(p1, p2, false, false) {
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

        // сразу проверяем на принадлежность отрезку, а потом будем фиксить бесконечные случаи
        // неравенство треугольника фактически
        if(IsZero((first - second).norm() - (first - dot).norm() - (second - dot).norm())) {
            return true;
        }

        // проверка по уравнению
        if(IsHorizontal()) {
            if(!IsZero(dot.y - first.y)) {
                return false;
            }
        }
        else if(IsVertical()) {
            if(!IsZero(dot.x - first.x)) {
                return false;
            }
        }
        else if(!IsZero(dot.y - dot.x * Tilt() - Intercept())) {
            return false;
        }

        // просто прямая
        if(fst_endless && scd_endless) {
            return true;
        }
        // если это луч, то проекция должна быть положительна
        else if(fst_endless) {
            return (dot - second).scalar(first - second) > 0;
        }
        else if(scd_endless) {
            return (dot - first).scalar(second - first) > 0;
        }
    }

    bool HasInfinitPoint() const {
        return fst_endless || scd_endless;
    }

    // делит линию по точке ray_start так, чтобы на ней остался отрезок [ray_start, ray_point]
    line Split(const point &ray_start, const point &ray_point) const {
        line left = line(ray_start, first, false, fst_endless);
        line right = line(ray_start, second, false, scd_endless);
        if(left.IsOnLine(ray_point)) {
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
        return {second, first, scd_endless, fst_endless};
    }

    // k1 * k2 = -1
    // "endless" bisector line
    line BisectorLine() const {
        point middle = Middle();
        if(IsVertical()) {
            // перендикуляр горизонтальный
            point bis1(0, middle.y);
            point bis2(10, middle.y);
            return {bis1, bis2, true, true};
        }

        if(IsHorizontal()) {
            // перпендикуляр вертикальный
            point bis1(middle.x, 0);
            point bis2(middle.x, 10);
            return {bis1, bis2, true, true};
        }

        double tilt = -1.0 / Tilt();
        double intercept = middle.y - middle.x * tilt;
        return {point(0, intercept), point(10, 10 * tilt + intercept), true, true};
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
        for(int i = static_cast<int>(end) - 2;  i > -1; --i) {
            while(curr > optimal_size && !point::IsCounter(sorted_list[hull[curr - 2]], sorted_list[hull[curr - 1]], sorted_list[i])) {
                --curr;
            }
            hull[curr] = static_cast<size_t >(i);
            ++curr;
        }
        // hull is not cycled
        hull.resize(curr - 1);
    }

    const size_t &operator[](size_t index) {
        return hull[index];
    }

    size_t size() const {
        return hull.size();
    }

    // clock
    size_t prev(size_t index) const {
        return index == 0 ? size() : index - 1;
    }

    // counterclock
    size_t next(size_t index) const {
        return index == size() - 1 ? 0 : index + 1;
    }

    static std::pair<size_t, size_t> LowestCommonSupport(const ConvexAndrew &voron, const ConvexAndrew &eagle, const std::vector<point> &point_set) {
        size_t left = voron.XMax(point_set);
        size_t right = eagle.XMin(point_set);
        bool change = true;
        while(change) {
            change = false;
            size_t lnext = voron.prev(left);
            size_t rnext = eagle.next(right);
            if(!point::IsCounter(point_set[left], point_set[right], point_set[lnext])) {
                change = true;
                left = lnext;
            }
            if(!point::IsCounter(point_set[left], point_set[right], point_set[rnext])) {
                change = true;
                right = rnext;
            }
        }
        return {left, right};
    }

    static std::pair<size_t, size_t> UpperCommonSupport(const ConvexAndrew &voron, const ConvexAndrew &eagle, const std::vector<point> &point_set) {
        size_t left = voron.XMax(point_set);
        size_t right = eagle.XMin(point_set);
        bool change = true;
        while(change) {
            change = false;
            size_t lnext = voron.next(left);
            size_t rnext = eagle.prev(right);
            if(point::IsCounter(point_set[left], point_set[right], point_set[lnext])) {
                change = true;
                left = lnext;
            }
            if(point::IsCounter(point_set[left], point_set[right], point_set[rnext])) {
                change = true;
                right = rnext;
            }
        }
        return {left, right};
    }

    size_t XMin(const std::vector<point> &point_set) const {
        size_t index = 0;
        for(size_t i = 1; i < hull.size(); ++i) {
            if(point_set[hull[i]].x < point_set[hull[index]].x) {
                index = i;
            }
        }
        return index;
    }

    size_t XMax(const std::vector<point> &point_set) const {
        size_t index = 0;
        for(size_t i = 1; i < hull.size(); ++i) {
            if(point_set[hull[i]].x > point_set[hull[index]].x) {
                index = i;
            }
        }
        return index;
    }

private:
    std::vector<size_t> hull;
};

struct edge {
    line direction;
    size_t site;
    // это очень тонкий момент = трюк с памятью и вообще магия
    edge *opposite = nullptr;
    edge *next = nullptr;
    // магия
    explicit edge(const line &linenin, size_t web) : direction(linenin), site(web) {
        next = this;
    }
};

class VoronoiDiargam {
public:
    // end - excluding
    explicit VoronoiDiargam(const std::vector<point> &points, std::vector<edge*> &entry, size_t begin, size_t end)
    : point_set(points), entry_edge(entry) {
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
        VoronoiDiargam voron(point_set, entry_edge, begin, split_key);
        VoronoiDiargam eagle(point_set, entry_edge, split_key, end);
        Merge(*this, voron, eagle, begin, end);
    }

    size_t Average() {
        ConvexAndrew andrew(point_set, 0, point_set.size());
        size_t limited_polygons = point_set.size() - andrew.size();
        size_t limited_edges = 0;
        std::vector<size_t> polygons(point_set.size(), 0);
        for(edge *entry : entry_edge) {
            edge *iter = entry;
            do {
                ++limited_edges;
                ++polygons[iter->site];
                // тут очищаю память, хз, должно работать
                edge *kek = iter;
                iter = iter->next;
                delete kek;
            } while(iter != entry);
        }
        for(int i = 0; i < andrew.size(); ++i) {
            limited_edges -= polygons[andrew[i]];
        }
        return (limited_polygons == 0 ? 0 :limited_edges / limited_polygons);
    }
private:
    VoronoiDiargam Merge(const VoronoiDiargam &result, const VoronoiDiargam &voron, const VoronoiDiargam &eagle,
            size_t begin, size_t end) {
        size_t split_key = (begin + end) / 2;
        // step 1
        ConvexAndrew convex_voron(point_set, begin, split_key);
        ConvexAndrew convex_eagle(point_set, split_key, end);

        // step 2
        auto lower = ConvexAndrew::LowestCommonSupport(convex_voron, convex_eagle, point_set);
        auto upper = ConvexAndrew::UpperCommonSupport(convex_voron, convex_eagle, point_set);

        auto zig_zag = upper;
        std::vector<line> chain;
        std::vector<std::pair<size_t, size_t> > border;
        // step 3
        // бесконечное ребро сверху
        line bisector = line(point_set[upper.first], point_set[upper.second]).BisectorLine();
        edge *left = Intersect(zig_zag.first, bisector);
        edge *right = Intersect(zig_zag.second, bisector);
        point left_inter = left == nullptr ? point() : line::Intersect(left->direction, bisector);
        point right_inter = right == nullptr ? point() : line::Intersect(right->direction, bisector);
        point upper_point;
        border.emplace_back(zig_zag);


        if(std::isnan(left_inter.y) && std::isnan(right_inter.y)) {
            // коллинеарные сайты, по условию такого быть не должно по идее, пока хэндла не придумал
            throw;
        }
        else if(std::isnan(right_inter.y) || (!std::isnan(left_inter.y) && left_inter.y > right_inter.y)) {
            upper_point = left_inter;
            zig_zag.first = left->opposite->site;
        }
        else {
            upper_point = right_inter;
            zig_zag.second = right->opposite->site;
        }
        // добавляем верхний луч в цепь
        chain.emplace_back(line(upper_point, point((upper_point.y + 10 - bisector.Intercept()) / bisector.Tilt(), upper_point.y + 10), false, true));

        // step 4
        while(zig_zag != lower) {

        }
        // step 5

        // step 6

    }

    // returns the first one edge from the above (top y-coord) to be intersected by line parameter
    edge *Intersect(size_t site, const line &inter) {
        double y = -1000000;
        edge *top_edge = nullptr;
        edge *entry = entry_edge[site];
        edge *iter = entry;
        do {
            point inter_dot = line::Intersect(iter->direction, inter);
            if(!std::isnan(inter_dot.y) && inter_dot.y > y &&
                iter->direction.IsOnLine(inter_dot) && inter.IsOnLine(inter_dot)) {
                top_edge = iter;
            }
            iter = iter->next;
        } while (iter != entry);
        return top_edge;
    }

    void InsertEdges(size_t site, const line &a, const line &b) {
        entry_edge[site] = new edge(a, site);
        entry_edge[site]->next = new edge(b, site);
        entry_edge[site]->next->next = entry_edge[site];
    }

    void MakeOpposite(edge* a, edge *b) {
        a->opposite = b;
        b->opposite = a;
    }

    void Build2(size_t begin, size_t end) {
        line temp = line(point_set[begin], point_set[begin + 1]);
        line bisector = temp.BisectorLine();
        if(point::IsCounter(temp.first, temp.second, bisector.second)) {
            entry_edge[begin] = new edge(bisector, begin);
            entry_edge[begin + 1] = new edge(bisector.Sym(), begin + 1);
        }
        else {
            entry_edge[begin] = new edge(bisector.Sym(), begin);
            entry_edge[begin + 1] = new edge(bisector, begin + 1);
        }
        MakeOpposite(entry_edge[begin], entry_edge[begin + 1]);
    }

    // центр описанной около треугольника окружности
    // пересечение двух серединных перпендикуляров
    void Build3(size_t begin, size_t end) {
        line ab(point_set[begin], point_set[begin + 1]);
        line bc(point_set[begin + 1], point_set[begin + 2]);
        line ca(point_set[begin + 2], point_set[begin]);

        line bis1 = ab.BisectorLine();
        line bis2 = bc.BisectorLine();
        line bis3 = ca.BisectorLine();

        // это же и есть вершина диаграмы
        point circle_center = line::Intersect(bis1, bis2);

        // теперь нужно построить три луча от центра окружности
        // если какой-то угол тупой, то нужно брать луч не содержащий точку центра стороны против тупого угла
        // точки же не коллинеарны
        line ray1 = bis1.Split(circle_center, ab.Middle());
        line ray2 = bis2.Split(circle_center, bc.Middle());
        line ray3 = bis3.Split(circle_center, ca.Middle());

        if(ca.norm() + bc.norm() - ab.norm() < 0) {
            ray1 = bis1.Split(circle_center, circle_center * 2 - ab.Middle());
        }
        else if(ab.norm() + ca.norm() - bc.norm() < 0) {
            ray2 = bis2.Split(circle_center, circle_center * 2 - bc.Middle());
        }
        else if(bc.norm() + ab.norm() - ca.norm() < 0) {
            ray3 = bis3.Split(circle_center, circle_center * 2 - ca.Middle());
        }

        // теперь нужно правильно вставить два луча в каждый полигон - против часовой стрелки
        // так как точки отсорчены по x, вообще говоря есть два случая: вторая точка над прямой первой-третьей или нет
        if(point::IsCounter(point_set[begin], point_set[begin + 2], point_set[begin + 1])) {
            InsertEdges(begin, ray3.Sym(), ray1);
            InsertEdges(begin + 1, ray1.Sym(), ray2);
            InsertEdges(begin + 2, ray2.Sym(), ray3);

            MakeOpposite(entry_edge[begin], entry_edge[begin + 2]->next);
            MakeOpposite(entry_edge[begin]->next, entry_edge[begin + 1]);
            MakeOpposite(entry_edge[begin + 2], entry_edge[begin + 1]->next);
        }
        else {
            InsertEdges(begin, ray1.Sym(), ray3);
            InsertEdges(begin + 1, ray2.Sym(), ray1);
            InsertEdges(begin + 2, ray3.Sym(), ray2);

            MakeOpposite(entry_edge[begin], entry_edge[begin + 1]->next);
            MakeOpposite(entry_edge[begin]->next, entry_edge[begin + 2]);
            MakeOpposite(entry_edge[begin + 1], entry_edge[begin + 2]->next);
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
    // это не переменные класса, а лишь ссылка - магия да и только
    edge *border;
    std::vector<edge*> entry_edge;
    const std::vector<point> &point_set;
};

void prepare_data(std::vector<point> &data) {
    double x;
    double y;
    size_t counter = 0;
    while(counter < 3 && std::cin >> x) {
        std::cin >> y;
        data.emplace_back(point(x, y));
        ++counter;
    }
    // sorting by x-coord for Divide&Conquer + Andrew's monotone chain
    sort(data.begin(), data.end());
}

bool InCircle(const point &a, const point &b, const point &c, const point &d) {
    // считаем суперопределитель
    double part1 = a.x * (b.y * (c.norm() - d.norm()) - c.y * (b.norm() - d.norm()) + d.y * (b.norm() - c.norm()) );
    double part2 = b.x * (a.y * (c.norm() - d.norm()) - c.y * (a.norm() - d.norm()) + d.y * (a.norm() - c.norm()) );
    double part3 = c.x * (a.y * (b.norm() - d.norm()) - b.y * (a.norm() - d.norm()) + d.y * (a.norm() - b.norm()) );
    double part4 = d.x * (a.y * (b.norm() - c.norm()) - b.y * (a.norm() - c.norm()) + c.y * (a.norm() - b.norm()) );
    return part1 - part2 + part3 - part4 > 0;
}

double line::comparing_precision = 1e-10;
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::vector<point> dots;
    prepare_data(dots);
    std::vector<edge*> entry_edge(dots.size());
    VoronoiDiargam voron(dots, entry_edge, 0, dots.size());

    std::cout << voron.Average();
    return 0;
}