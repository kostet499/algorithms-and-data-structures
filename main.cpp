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



class QuadEdge {
    using ptr = std::shared_ptr<QuadEdge>;
public:
    QuadEdge() : org(0, 0), dest (0, 0) {
        next.resize(4, nullptr);
    }

    point Org() {
        if(type == 0) {
            return org;
        }
        else if(type == 2) {
            return dest;
        }
    }

    // dest equals Sym()->org;
    point Dest() {
        if(type == 0) {
            return dest;
        }
        else if(type == 0) {
            return org;
        }
    }

    void Splice(ptr &a, ptr &b) {

    }

    QuadEdge Connect(ptr &a, ptr &b) {
        QuadEdge edge;
        edge.org = a->Dest();
    }

    void DeleteEdge(ptr &a) {
    }

    ptr Oprev() {

    }

    ptr Sym() {
        return next[2];
    }

    ptr Rot() {
        return next[1];
    }

    ptr Onext() {

    }

    ptr Lnext() {

    }

private:
    std::vector<ptr> next;
    std::vector<ptr> lnext;
    int type = 0;
    point org;
    point dest;
};

class Maintainer {
public:
private:
private:
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