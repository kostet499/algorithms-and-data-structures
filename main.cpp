#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>
#include <list>
#include <fstream>

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
        return (!IsZero(this->x - b.x) && this->x < b.x) || (IsZero(this->x - b.x) && this->y < b.y);
    }
    bool operator==(const point &b) const {
        return IsZero((*this - b).norm());
    }
    static bool IsZero(double value) {
        return fabs(value) < comparing_precision;
    }
    double norm() const {
        return std::pow(this->x, 2) + std::pow(this->y, 2);
    }
    double dist() const {
        return std::sqrt(norm());
    }
    double scalar(const point &b) {
        return this->x * b.x + this->y * b.y;
    }
    point() : x(std::nan("")), y(std::nan("")) {
    }
    static double Angle(const point &a, const point &b);
    static bool IsCounter(const point &a, const point &b, const point &c);
    static double comparing_precision;
};

double point::comparing_precision = 1e-10;

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
                ((b.first == first && b.second == second) ||
                (b.first == second && b.second == first));
    }

    bool operator!=(const line &b) {
        return !(*this == b);
    }

    double norm() const {
        return (second - first).norm();
    }

    double dist() const {
        return (second - first).dist();
    }

    line(const point &p1, const point &p2, bool fst, bool scd) : first(p1), second(p2), fst_endless(fst), scd_endless(scd){
        if(p2 < p1) {
            std::swap(first, second);
            std::swap(fst_endless, scd_endless);
        }
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
        if(IsZero(dist() - (first - dot).dist() - (second - dot).dist())) {
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
        return false;
    }

    bool HasInfinitPoint() const {
        return fst_endless || scd_endless;
    }

    // делит линию по точке ray_start так, чтобы на ней остался отрезок [ray_start, ray_point]
    line Split(const point &ray_start, const point &ray_point) const {
        line left = line(ray_start, first, false, fst_endless);
        line right = line(ray_start, second, false, scd_endless);
        if(fst_endless) {
            left.second = ray_start + first - second;
        }

        if(scd_endless) {
            right.second = ray_start + second - first;
        }

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

    bool IsLeftRay(const line &other) const {
        point infinit = other.fst_endless ? other.first : other.second;
        point fst = fst_endless ? first : second;
        point scd = scd_endless ? first : second;
        return point::IsCounter(scd, fst, infinit);
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
        for(int i = static_cast<int>(end) - 2;  i >= static_cast<int>(begin); --i) {
            while(curr > optimal_size && !point::IsCounter(sorted_list[hull[curr - 2]], sorted_list[hull[curr - 1]], sorted_list[i])) {
                --curr;
            }
            hull[curr] = static_cast<size_t>(i);
            ++curr;
        }
        // hull is not cycled
        hull.resize(curr - 1);
    }

    const size_t &operator[](size_t index) const {
        return hull[index];
    }

    size_t size() const {
        return hull.size();
    }

    // clock
    size_t prev(size_t index) const {
        return index == 0 ? size() - 1 : index - 1;
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
            if(!point::IsCounter(point_set[voron[left]], point_set[eagle[right]], point_set[voron[lnext]])) {
                change = true;
                left = lnext;
            }
            if(!point::IsCounter(point_set[voron[left]], point_set[eagle[right]], point_set[eagle[rnext]])) {
                change = true;
                right = rnext;
            }
        }
        return {voron[left], eagle[right]};
    }

    static std::pair<size_t, size_t> UpperCommonSupport(const ConvexAndrew &voron, const ConvexAndrew &eagle, const std::vector<point> &point_set) {
        size_t left = voron.XMax(point_set);
        size_t right = eagle.XMin(point_set);
        bool change = true;
        while(change) {
            change = false;
            size_t lnext = voron.next(left);
            size_t rnext = eagle.prev(right);
            if(point::IsCounter(point_set[voron[left]], point_set[eagle[right]], point_set[voron[lnext]])) {
                change = true;
                left = lnext;
            }
            if(point::IsCounter(point_set[voron[left]], point_set[eagle[right]], point_set[eagle[rnext]])) {
                change = true;
                right = rnext;
            }
        }
        return {voron[left], eagle[right]};
    }

    size_t XMin(const std::vector<point> &point_set) const {
        size_t index = 0;
        for(size_t i = 1; i < hull.size(); ++i) {
            if(hull[i] < hull[index]) {
                index = i;
            }
        }
        return index;
    }

    size_t XMax(const std::vector<point> &point_set) const {
        size_t index = 0;
        for(size_t i = 1; i < hull.size(); ++i) {
            if(hull[i] > hull[index]) {
                index = i;
            }
        }
        return index;
    }

private:
    std::vector<size_t> hull;
};

class edge {
public:
    line direction;
    size_t site;
    // это очень тонкий момент = трюк с памятью и вообще магия
    edge *opposite = nullptr;
    edge *next = nullptr;
    // магия
    explicit edge(const line &linenin, size_t web) : direction(linenin), site(web) {
    }

    edge() : direction(point(), point()), site(0) {
    }

    void Change(const line &a) {
        direction = a;
        opposite->direction = a;
    }

    void Next(edge *new_next) {
        if(next == new_next || new_next == nullptr) {
            throw;
        }
        else {
            next = new_next;
        }
    }
};

class VoronoiDiargam {
public:
    // end - excluding
    explicit VoronoiDiargam(const std::vector<point> &points, std::vector<edge*> &entry, size_t begin, size_t end)
    : point_set(points), entry_edge(entry) {
        storage.resize(200000);
        Build(begin, end);
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
                iter = iter->next;
            } while(iter != entry);
        }
        for(int i = 0; i < andrew.size(); ++i) {
            limited_edges -= polygons[andrew[i]];
        }
        return (limited_polygons == 0 ? 0 :limited_edges / limited_polygons);
    }

    void LoadCSV(const char *filename) {
        std::ofstream out;
        out.open(filename);
        out << "fx;fy;sx;sy;fst;scd" << std::endl;
        for(edge *entry : entry_edge) {
            edge *iter = entry;
            do {
                out << iter->direction.first.x << ";" << iter->direction.first.y << ";";
                out << iter->direction.second.x << ";" << iter->direction.second.y << ";";
                out << static_cast<int>(iter->direction.fst_endless) << ";" << static_cast<int>(iter->direction.scd_endless);

                out << std::endl;
                iter = iter->next;
            } while(iter != entry);
        }
        out.close();
    }
private:
    void Build(size_t begin, size_t end) {
        if(end - begin == 2) {
            Build2(begin, begin + 1);
            return;
        }
        if(end - begin == 3) {
            Build3(begin, end);
            return;
        }
        size_t split_key = (begin + end) / 2;
        Build(begin, split_key);
        Build(split_key, end);
        Merge(begin, end, split_key);
    }

    void Merge(size_t begin, size_t end, size_t split_key) {
        // step 1
        ConvexAndrew convex_voron(point_set, begin, split_key);
        ConvexAndrew convex_eagle(point_set, split_key, end);

        // step 2
        auto lower = ConvexAndrew::LowestCommonSupport(convex_voron, convex_eagle, point_set);
        auto upper = ConvexAndrew::UpperCommonSupport(convex_voron, convex_eagle, point_set);

        auto zig_zag = upper;
        std::vector<line> chain;
        std::vector<std::pair<size_t, size_t> > border;
        std::vector<std::pair<point, edge*> > cross;
        // step 3
        // луч сверху
        line bisector = line(point_set[upper.first], point_set[upper.second]).BisectorLine();

        border.emplace_back(zig_zag);
        auto diech = ChainStep(bisector, zig_zag, true, point());
        point upper_point = diech.first;
        if(std::isnan(upper_point.y)) {
            // коллинеарный случай - значит мы можем просто запустить Build2
            Build2(zig_zag.first, zig_zag.second);
            return;
        }
        // добавляем верхний луч в цепь
        chain.emplace_back(upper_point, point((upper_point.y + 10 - bisector.Intercept()) / bisector.Tilt(), upper_point.y + 10), false, true);
        cross.emplace_back(diech);
        // step 4
        while(zig_zag != lower) {
            bisector = line(point_set[zig_zag.first], point_set[zig_zag.second]).BisectorLine();

            border.emplace_back(zig_zag);
            diech = ChainStep(bisector, zig_zag, false, upper_point);
            point down_point = diech.first;

            chain.emplace_back(down_point, upper_point);
            cross.emplace_back(diech);
            upper_point = down_point;
        }
        // step 5
        bisector = line(point_set[zig_zag.first], point_set[zig_zag.second]).BisectorLine();

        border.emplace_back(zig_zag);
        // no ChainStep here, because upper_point is just what we need now
        // луч снизу
        chain.emplace_back(point((upper_point.y - 10 - bisector.Intercept()) / bisector.Tilt(), upper_point.y - 10),
                upper_point, true, false);
        cross.emplace_back(upper_point, cross.back().second->opposite);
        // step 6
        // duck the sick / sosipisos /
        // предполагается, что полигональная кривая
        // против часовой стрелки для ворона
        std::vector<edge*> left_edges(chain.size());
        size_t left_top;
        for(left_top = 1; left_top < border.size(); ++left_top) {
            if(border[left_top].first != border[left_top - 1].first){
                break;
            }
        }
        --left_top;

        left_edges[0] = newEdge(chain[0], border[0].first);
        left_edges.front()->Next(GiveRay(border[0].first, true));
        entry_edge[border[left_top].first] = cross[left_top].second;
        cross[left_top].second->Change(
                line(cross[left_top].second->direction.first, cross[left_top].first,
                        cross[left_top].second->direction.fst_endless, false));
        for(size_t i = 1; i < left_top + 1; ++i) {
            left_edges[i] = newEdge(chain[i], border[i].first);
            left_edges[i]->Next(left_edges[i - 1]);
        }
        cross[left_top].second->Next(left_edges[left_top]);
        ++left_top;
        // inside
        while(border[left_top].first != border.back().first) {
            size_t left_bot;
            for(left_bot = left_top + 1; left_bot < border.size(); ++left_bot) {
                if(border[left_bot].first != border[left_top].first) {
                    break;
                }
            }
            --left_bot;
            edge *top_inter = cross[left_top - 1].second->opposite;
            edge *bot_inter = cross[left_bot].second;
            entry_edge[border[left_top].first] = top_inter;
            bot_inter->Change(line(bot_inter->direction.first, cross[left_bot].first));
            if(bot_inter->next != top_inter) {
                throw;
            }
            left_edges[left_top] = newEdge(chain[left_top], border[left_top].first);
            left_edges[left_top]->Next(top_inter);
            for(size_t i = left_top + 1; i < left_bot + 1; ++i) {
                left_edges[i] = newEdge(chain[i], border[i].first);
                left_edges[i]->Next(left_edges[i - 1]);
            }
            bot_inter->Next(left_edges[left_bot]);

            left_top = left_bot + 1;
        }
        // outside
        edge *last_ray = GiveRay(border[left_top].first, false);
        edge *out_inter = cross[left_top - 1].second->opposite;
        entry_edge[border[left_top].first] = out_inter;
        left_edges[left_top] = newEdge(chain[left_top], border[left_top].first);
        left_edges[left_top]->Next(out_inter);
        for(size_t i = left_top + 1; i < cross.size(); ++i) {
            left_edges[i] = newEdge(chain[i], border[i].first);
            left_edges[i]->Next(left_edges[i - 1]);
        }
        last_ray->Next(left_edges.back());

        // now eagle refresh
        std::vector<edge*> right_edges(chain.size());
        size_t right_top;
        for(right_top = 1; right_top < border.size(); ++right_top) {
            if(border[right_top].second != border[right_top - 1].second){
                break;
            }
        }
        --right_top;

        right_edges[0] = newEdge(chain[0], border[0].second);
        last_ray = GiveRay(border[right_top].second, false);
        last_ray->Next(right_edges.front());
        entry_edge[border[right_top].second] = cross[right_top].second;
        cross[right_top].second->Change(line(cross[right_top].second->direction.second, cross[right_top].first,
                cross[right_top].second->direction.scd_endless, false));
        for(size_t i = 1; i < right_top + 1; ++i) {
            right_edges[i] = newEdge(chain[i], border[i].second);
            right_edges[i - 1]->Next(right_edges[i]);
        }
        right_edges[right_top]->Next(cross[right_top].second);

        ++right_top;
        // inside
        while(border[right_top].second != border.back().second) {
            size_t right_bot;
            for(right_bot = right_top + 1; right_bot < border.size(); ++right_bot) {
                if(border[right_bot].second != border[right_top].second) {
                    break;
                }
            }
            --right_bot;
            edge *top_inter = cross[right_top - 1].second->opposite;
            edge *bot_inter = cross[right_bot].second;
            entry_edge[border[right_top].second] = top_inter;
            bot_inter->Change(line(bot_inter->direction.second, cross[right_bot].first));
            if(cross[right_top].second->next != cross[right_bot].second) {
                throw;
            }
            right_edges[right_top] = newEdge(chain[right_top], border[right_top].second);
            top_inter->Next(right_edges[right_top]);
            for(size_t i = right_top + 1; i < right_bot + 1; ++i) {
                right_edges[i] = newEdge(chain[i], border[i].second);
                right_edges[i - 1]->Next(right_edges[i]);
            }
            right_edges[right_bot]->Next(bot_inter);

            right_top = right_bot + 1;
        }
        // outside
        last_ray = GiveRay(border[right_top].second, true);
        out_inter = cross[right_top - 1].second->opposite;
        entry_edge[border[right_top].second] = out_inter;
        right_edges[right_top] = newEdge(chain[right_top], border[right_top].second);
        out_inter->Next(right_edges[right_top]);
        for(size_t i = right_top + 1; i < cross.size(); ++i) {
            right_edges[i] = newEdge(chain[i], border[i].second);
            right_edges[i - 1]->Next(right_edges[i]);
        }
        right_edges.back()->Next(last_ray);

        // link voron and eagle new edges
        for(size_t i = 0; i < left_edges.size(); ++i) {
            if(left_edges[i]->direction != right_edges[i]->direction) {
                throw;
            }
            MakeOpposite(left_edges[i], right_edges[i]);
        }
    }

    // left = false - выдаст правый луч неогр. сайта, иначе левый
    edge *GiveRay(size_t site, bool left) const {
        edge *found_ray = nullptr;
        edge *iter = entry_edge[site];
        do {
            if(iter->direction.HasInfinitPoint()) {
                if(found_ray == nullptr) {
                    found_ray = iter;
                }
                else {
                    if(iter->direction.IsLeftRay(found_ray->direction) == left) {
                        return found_ray;
                    }
                    else {
                        return iter;
                    }
                }
            }
            iter = iter->next;
        } while(iter != entry_edge[site]);
        return found_ray;
    }

    std::pair <point, edge*> ChainStep(const line &bisector, std::pair<size_t, size_t> &zig_zag, bool up, const point &last) const {
        edge *left;
        edge *right;
        if(up) {
            left = IntersectUp(zig_zag.first, bisector);
            right = IntersectUp(zig_zag.second, bisector);
        }
        else {
            left = IntersectDown(zig_zag.first, bisector);
            right = IntersectDown(zig_zag.second, bisector);
        }
        point left_inter = left == nullptr ? point() : line::Intersect(left->direction, bisector);
        point right_inter = right == nullptr ? point() : line::Intersect(right->direction, bisector);
        if(left_inter == last) {
            zig_zag.second = right->opposite->site;
            return {right_inter, right};
        }
        else if(right_inter == last) {
            zig_zag.first = left->opposite->site;
            return {left_inter, left};
        }
        if(std::isnan(left_inter.y) && std::isnan(right_inter.y)) {
            // коллинеарные сайты, по условию такого быть не должно по идее, пока хэндла не придумал
            return {};
        }
        else if(std::isnan(right_inter.y) || (!std::isnan(left_inter.y) &&
                ((line::IsZero(left_inter.y  - right_inter.y) && left_inter.x > right_inter.x) ||
                        (!line::IsZero(left_inter.y - right_inter.y) && left_inter.y > right_inter.y)) )) {
            zig_zag.first = left->opposite->site;
            return {left_inter, left};
        }
        else {
            zig_zag.second = right->opposite->site;
            return {right_inter, right};
        }
    }

    // returns the first one edge from the above (top y-coord) to be intersected by line parameter
    edge *IntersectUp(size_t site, const line &inter) const {
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

    edge *IntersectDown(size_t site, const line &inter) const {
        double y = 1000000;
        edge *top_edge = nullptr;
        edge *entry = entry_edge[site];
        edge *iter = entry;
        do {
            point inter_dot = line::Intersect(iter->direction, inter);
            if(!std::isnan(inter_dot.y) && inter_dot.y < y &&
               iter->direction.IsOnLine(inter_dot) && inter.IsOnLine(inter_dot)) {
                top_edge = iter;
            }
            iter = iter->next;
        } while (iter != entry);
        return top_edge;
    }

    void InsertEdges(size_t site, const line &a, const line &b) {
        entry_edge[site] = newEdge(a, site);
        entry_edge[site]->next = newEdge(b, site);
        entry_edge[site]->next->next = entry_edge[site];
    }

    void MakeOpposite(edge* a, edge *b) {
        a->opposite = b;
        b->opposite = a;
    }

    edge *newEdge(const line &liny, size_t site) {
        storage[storage_index] = edge(liny, site);
        storage[storage_index].next = &storage[storage_index];
        ++storage_index;
        return &storage[storage_index - 1];
    }

    void Build2(size_t first, size_t second) {
        line temp = line(point_set[first], point_set[second]);
        line bisector = temp.BisectorLine();
        entry_edge[first] = newEdge(bisector, first);
        entry_edge[second] = newEdge(bisector, second);
        MakeOpposite(entry_edge[first], entry_edge[second]);
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
            InsertEdges(begin, ray3, ray1);
            InsertEdges(begin + 1, ray1, ray2);
            InsertEdges(begin + 2, ray2, ray3);

            MakeOpposite(entry_edge[begin], entry_edge[begin + 2]->next);
            MakeOpposite(entry_edge[begin]->next, entry_edge[begin + 1]);
            MakeOpposite(entry_edge[begin + 2], entry_edge[begin + 1]->next);
        }
        else {
            InsertEdges(begin, ray1, ray3);
            InsertEdges(begin + 1, ray2, ray1);
            InsertEdges(begin + 2, ray3, ray2);

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

    // можем нумеровать сайты в естественном порядке (по сортировке по x-коориднате, чтобы связать с нумерацией в массиве)
    // это не переменные класса, а лишь ссылка - магия да и только
    std::vector<edge*> &entry_edge;
    const std::vector<point> &point_set;
    std::vector<edge> storage;
    size_t storage_index = 0;
};

void prepare_data(std::vector<point> &data) {
    double x;
    double y;
    size_t counter = 0;
    while(counter < 3 && std::cin >> x) {
        std::cin >> y;
        data.emplace_back(x, y);
        ++counter;
    }
    // sorting by x-coord for Divide&Conquer + Andrew's monotone chain
    sort(data.begin(), data.end());
}

void prepare_test_data(std::vector<point> &data, const char *filename) {
    double x;
    double y;
    std::ifstream in;
    in.open(filename);
    char c;
    in >> c;
    in >> c;
    while(in >> x) {
        in >> y;
        data.emplace_back(x, y);
    }
    in.close();
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
    //prepare_data(dots);
    prepare_test_data(dots, "test");
    std::vector<edge*> entry_edge(dots.size());
    VoronoiDiargam voron(dots, entry_edge, 0, dots.size());

    voron.LoadCSV("visualisation_data.csv");

    std::cout << voron.Average();
    return 0;
}