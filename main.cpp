#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;
struct point {
    double x, y, z;
    point(double v1, double v2, double v3) : x(v1), y(v2), z(v3){}
    point operator+(const point &b) const {
        return {x + b.x, y + b.y, z + b.z};
    }
    point operator*(double number) const {
        return {x * number, y * number, z * number};
    }
    point operator[](const point &b) const {
        return {y * b.z - z * b.y, - x * b.z + z * b.x, x * b.y - y * b.x};
    }
    point operator-(const point &b) const {
        return {x - b.x, y - b.y, z - b.z};
    }
    double operator()(const point &b) const {
        return x * b.x + y * b.y + z * b.z;
    }
    double operator()() const {
        return operator()(*this);
    }
};

double angle(const point &a, const point &b) {
    return a(b) / sqrt(a()) / sqrt(b());
}

double compute_distance(const point &dot, const point &a, const point &b) {
    auto direction = b - a;
    auto touch = dot - a;

    auto angle_cos = angle(touch, direction);
    auto parameter = touch(direction) / direction();

    if(direction() < 1e-8 || 0.0 > parameter) {
        auto difference = a - dot;
        return sqrt(difference());
    }

    if(parameter > 1) {
        auto difference = b - dot;
        return sqrt(difference());
    }

    // our point is on the line
    // returns length of touch * sin(angle between touch and direction)
    double sq_cos = pow(angle_cos, 2);
    double sinus = sq_cos > 1 ? 0 : sqrt(1 - sq_cos);

    return sqrt(touch()) * sinus;
}

double result_distance(const vector<point> &coord) {
    double mid = 0.0;
    double left = 0.0;
    double right = 1.0;
    double compare_precision = 1e-10;
    auto direction = coord[1] - coord[0];

    while(right - left > compare_precision) {
        mid = (left + right) / 2;

        auto fst = coord[0] + direction * mid;
        auto scd = fst + direction * compare_precision;

        auto distance_difference = compute_distance(fst, coord[2], coord[3]) - compute_distance(scd, coord[2], coord[3]);
        if(distance_difference < 0) {
            right = mid;
        }
        else {
            left = mid;
        }
    }
    return compute_distance(coord[0] + direction * mid, coord[2], coord[3]);
}

int main() {
    vector<point> coord(4, point(0, 0, 0));
    for(int i = 0; i < 4; ++i) {
        double x, y, z;
        cin >> x >> y >> z;
        coord[i] = point(x, y, z);
    }
    printf("%0.7f", result_distance(coord));
    return 0;
}
