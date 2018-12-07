#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::sqrt;

vector<double> minus(const vector<double> &a, const vector<double> &b) {
    vector<double> res(a.size(), 0.0);
    for(int i = 0; i < a.size(); ++i) {
        res[i] = a[i] - b[i];
    }
    return res;
}

vector<double> plus(const vector<double> &a, const vector<double> &b) {
    vector<double> res(a.size(), 0.0);
    for(int i = 0; i < a.size(); ++i) {
        res[i] = a[i] + b[i];
    }
    return res;
}

vector<double> multiply(const vector<double> &a, double b) {
    vector<double> res(a.size(), 0.0);
    for(int i = 0; i < a.size(); ++i) {
        res[i] = a[i] * b;
    }
    return res;
}

double scalar(const vector<double> &a, const vector<double> &b) {
    double res = 0.0;
    for(int i = 0; i < a.size(); ++i) {
        res += a[i] * b[i];
    }
    return res;
}

double norm(const vector<double> &a) {
    double sum = 0.0;
    for(auto &element : a) {
        sum += pow(element, 2);
    }
    return sqrt(sum);
}

double angle(const vector<double> &a, const vector<double> &b) {
    return scalar(a, b) / norm(a) / norm(b);
}

double compute_distance(const vector<double> &point, const vector<double> &a, const vector<double> &b) {
    auto direction = minus(b, a);
    auto touch = minus(point, a);

    auto angle_cos = angle(touch, direction);
    auto parameter = scalar(touch, direction) / pow(norm(direction), 2);

    if(norm(direction) < 1e-8 || 0.0 > parameter) {
        auto difference = minus(a, point);
        return norm(difference);
    }

    if(parameter > 1) {
        auto difference = minus(b, point);
        return norm(difference);
    }

    // our point is on the line
    // returns length of touch * sin(angle between touch and direction)
    double sq_cos = pow(angle_cos, 2);
    double sinus = sq_cos > 1 ? 0 : sqrt(1 - sq_cos);

    return norm(touch) * sinus;
}

double result_distance(const vector<vector<double > > &coord) {
    double t = 0.0;
    double left = 0.0;
    double right = 1.0;
    double prec = 1e-10;
    double step_prec = 1e-8;

    auto direction = minus(coord[1], coord[0]);


    while(right - left > prec) {
        t = (left + right) / 2;

        auto shift = multiply(direction, t);
        auto fst = plus(coord[0], shift);

        auto e_shift = multiply(direction, t + step_prec);
        auto scd = plus(coord[0], e_shift);

        auto distance_difference = compute_distance(fst, coord[2], coord[3]) - compute_distance(scd, coord[2], coord[3]);
        if(distance_difference < 0) {
            right = t + step_prec;
        }
        else {
            left = t;
        }

        step_prec = step_prec < (right - left) / 10.0 ? step_prec : step_prec / 10;
        //cout << step_prec << endl;
    }

    auto best_shift = multiply(direction, t);
    auto best_point = plus(coord[0], best_shift);
    return compute_distance(best_point, coord[2], coord[3]);
}

int main() {
    vector<vector<double > > coord(4, vector<double>(3, 0));

    for(int i = 0; i < 4; ++i) {
        cin >> coord[i][0] >> coord[i][1] >> coord[i][2];
    }

    double answer = result_distance(coord);
    swap(coord[0], coord[3]);
    swap(coord[1], coord[2]);
    answer = std::min(result_distance(coord), answer);
    swap(coord[0], coord[1]);
    swap(coord[3], coord[2]);
    answer = std::min(result_distance(coord), answer);
    printf("%0.7f", result_distance(coord));

    return 0;
}
