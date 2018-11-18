#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>

/*
 * http://sccg.sk/~samuelcik/dgs/quad_edge.pdf
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
};

// without flip - simplified version
class QuadEdge {
    using ptr = std::shared_ptr<QuadEdge>;
    using record = std::pair<ptr, size_t>;
public:
    // subdivision of sphere
    QuadEdge() : org(0, 0), dest (0, 0) {
        next.resize(4, record(this, 0));
        lnext.resize(4, record(this, 0));
        next[0].second = 0;
        lnext[0].second = 2;
        next[1].second = 3;
        lnext[1].second = 1;
        next[2].second = 2;
        lnext[2].second = 0;
        next[3].second = 1;
        lnext[3].second = 3;
    }

    static record Rot(const record &rec) {
        return {rec.first, (rec.second + 1) & 3};
    }

    static record Sym(const record &rec) {
        return {rec.first, (rec.second + 2) & 3};
    }

    static record RevRot(const record &rec) {
        return {rec.first, (rec.second + 3) & 3};
    }

    static record Onext(const record &rec) {
        return {rec.first->next[rec.second].first, rec.first->next[rec.second].second};
    }

    static record Oprev(const record &rec) {
        return Rot(Onext(Rot(rec)));
    }

private:
    std::vector<record> next;
    std::vector<record> lnext;
    point org;
    point dest;
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

// точки a, b, c формируют треугольник с обходом против часовой стрелки
bool CCW(point a, point b, point c) {
    return a.x * (b.y - c.y) - b.x * (a.y - c.y) + c.x * (a.y - b.y) > 0;
}

void prepare_data(std::vector<point> &data) {
    double x;
    double y;
    while(std::cin >> x) {
        std::cin >> y;
        data.emplace_back(point(x, y));
    }
    sort(data.begin(), data.end());
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::vector<point> dots;
    prepare_data(dots);

    return 0;
}