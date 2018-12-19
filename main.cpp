#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <utility>

using namespace std;
struct dot {
    double first;
    double second;

    dot(double x, double y) : first(x), second(y) {
    }

    dot() : dot(0, 0) {
    }
};

dot operator+(const dot &a, const dot &b) {
    return {a.first + b.first, a.second + b.second};
}

dot operator-(const dot &a, const dot &b) {
    return {a.first - b.first, a.second - b.second};
}

double calcAngle(const dot &a, const dot &b) {
    // spizgeno
    // http://qaru.site/questions/143580/direct-way-of-computing-clockwise-angle-between-2-vectors
    double one = a.first * b.first + a.second * b.second;
    double two = a.first * b.second - a.second * b.first;
    return atan2(two, one);
}

double calcSquareNorm(const dot &a, const dot &b) {
    return pow(a.first - b.first, 2) + pow(a.second - b.second, 2);
}

class Convex {
public:
    Convex(const vector<dot> &value) : dots(value) {
    }

    // order is saved - we have just turned every vector for 180 degrees
    void reflect() {
        for(auto &point : dots) {
            point = dot(-point.first, -point.second);
        }
    }

    // the most left dot is chosen and then rectangle is shifted so this dot to become first
    void resort() {
        vector<dot> resorted(dots.size() + 2);
        size_t index = 0;
        for(size_t i = 1; i < dots.size(); ++i) {
            if(dots[i].first < dots[index].first || (dots[i].first == dots[index].first && dots[i].second < dots[index].second)) {
                index = i;
            }
        }

        for(size_t i = 0; i < dots.size(); ++i) {
            resorted[i] = dots[(index + i) % dots.size()];
        }

        resorted[resorted.size() - 2] = resorted[0];
        resorted[resorted.size() - 1] = resorted[1];

        dots = resorted;
    }

    friend Convex calcMinkovsky(const Convex &rect1, const Convex &rect2);

    // precision problems may occur when comparing doubles with 0
    bool checkForZero() const {
        // precision for comparing
        double comparing_precision = 1e-5;
        //cout << count_area(make_pair(0.0, 0.0), rect) << " " << count_area(rect[0], rect) << endl;
        return comparing_precision > countArea(dot(0.0, 0.0)) - countArea(dots[0]);
    }
private:
    const size_t size() const {
        return dots.size();
    }

    const dot& operator[] (size_t index) const {
        return dots[index];
    }

    double countArea(const dot &point) const {
        double area = 0.0;
        for(size_t j = 1; j < dots.size() + 1; ++j) {
            size_t i = j == dots.size() ? 0 : j;
            double a = calcSquareNorm(dots[j - 1], dots[i]);
            double b = calcSquareNorm(dots[j - 1], point);
            double c = calcSquareNorm(dots[i], point);

            double sq_cos_angle = pow(a + b - c, 2) / 4 / (a * b);
            if(b == 0 || a == 0 || fabs(sq_cos_angle) > 1.0) {
                sq_cos_angle = 1;
            }
            double sin_angle = sqrt(1 - sq_cos_angle);
            area += sin_angle / 2 * sqrt(a * b);
        }
        return area;
    }
private:
    vector<dot> dots;
};

// spizgeno
// neerc - Сумма Минковского
Convex calcMinkovsky(const Convex &rect1, const Convex &rect2) {
    Convex minkov({});
    double prec = 1e-5;
    // suppose that vertices are sorted by polar angle
    Convex rect1_resorted = rect1;
    rect1_resorted.resort();
    Convex rect2_resorted = rect2;
    rect2_resorted.reflect();
    rect2_resorted.resort();
    size_t index1 = 0;
    size_t index2 = 0;

    while(index1 < rect1.size() || index2 < rect2.size()) {
        minkov.dots.emplace_back(rect1_resorted[index1] + rect2_resorted[index2]);
        double ang = calcAngle(rect1_resorted[index1 + 1] - rect1_resorted[index1], rect2_resorted[index2 + 1] - rect2_resorted[index2]);
        if(fabs(ang) < prec) {
            ++index1;
            ++index2;
        }
        else if(ang < 0) {
            ++index1;
        }
        else {
            ++index2;
        }
    }

    assert(minkov.size() <= rect1.size() + rect2.size());
    return minkov;
}

int main() {
    size_t n, m;
    cin >> n;
    vector<dot> rect1(n);
    for (size_t i = 0; i < n; ++i) {
        cin >> rect1[i].first >> rect1[i].second;
    }

    cin >> m;
    vector<dot> rect2(m);
    for (size_t i = 0; i < m; ++i) {
        cin >> rect2[i].first >> rect2[i].second;
    }

    Convex convex1(rect1);
    Convex convex2(rect2);

    if(calcMinkovsky(convex1, convex2).checkForZero()) {
        cout << "YES";
    }
    else {
        cout << "NO";
    }
    return 0;
}
