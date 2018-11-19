#include <vector>
#include <iostream>
#include <utility>
#include <cmath>
#include <algorithm>
#include <assert.h>
using std::cout;
using std::cin;
using std::endl;
using std::make_pair;
using std::atan2;
using std::sqrt;

using dot = std::pair<double, double>;
using rectangle = std::vector<std::pair<double, double> >;

// order is saved - we have just turned every vector for 180 degrees
rectangle reflected_vector(const rectangle &rect) {
    rectangle reflected;
    for(auto &point : rect) {
        reflected.emplace_back(make_pair(-point.first, -point.second));
    }
    return reflected;
}

// the most left dot is chosen and then rectangle is shifted so this dot to become first
rectangle resort_rectangle(const rectangle &rect) {
    rectangle resorted(rect.size() + 2);
    size_t index = 0;
    for(size_t i = 1; i < rect.size(); ++i) {
        if(rect[i].first < rect[index].first || (rect[i].first == rect[index].first && rect[i].second < rect[index].second)) {
            index = i;
        }
    }

    for(size_t i = 0; i < rect.size(); ++i) {
        resorted[i] = rect[(index + i) % rect.size()];
    }

    resorted[resorted.size() - 2] = resorted[0];
    resorted[resorted.size() - 1] = resorted[1];

    return resorted;
}

dot plus(dot a, dot b) {
    dot c;
    c.first = a.first + b.first;
    c.second = a.second + b.second;
    return c;
}

dot minus(dot a, dot b) {
    dot c;
    c.first = a.first - b.first;
    c.second = a.second - b.second;
    return c;
}

double angle(dot a, dot b) {
    // spizgeno
    // http://qaru.site/questions/143580/direct-way-of-computing-clockwise-angle-between-2-vectors
    double one = a.first * b.first + a.second * b.second;
    double two = a.first * b.second - a.second * b.first;
    return atan2(two, one);
}

// spizgeno
// neerc - Сумма Минковского
rectangle minkovsky(const rectangle &rect1, const rectangle &rect2) {
    rectangle minkov;
    double prec = 1e-5;
    // suppose that vertices are sorted by polar angle
    rectangle rect1_resorted = resort_rectangle(rect1);
    rectangle rect2_resorted = resort_rectangle(rect2);
    size_t index1 = 0;
    size_t index2 = 0;

    while(index1 < rect1.size() || index2 < rect2.size()) {
        minkov.emplace_back(plus(rect1_resorted[index1], rect2_resorted[index2]));
        double ang = angle(minus(rect1_resorted[index1 + 1], rect1_resorted[index1]), minus(rect2_resorted[index2 + 1], rect2_resorted[index2]));
        //cout << angle1 << " " << angle2 << endl;
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

double sq_norm(dot a, dot b) {
    return pow(a.first - b.first, 2) + pow(a.second - b.second, 2);
}

double count_area(dot point, const rectangle &rect) {
    double area = 0.0;
    for(size_t j = 1; j < rect.size() + 1; ++j) {
        size_t i = j == rect.size() ? 0 : j;
        double a = sq_norm(rect[j - 1], rect[i]);
        double b = sq_norm(rect[j - 1], point);
        double c = sq_norm(rect[i], point);

        double sq_cos_angle = pow(a + b - c, 2) / 4 / (a * b);
        if(b == 0 || a == 0 || fabs(sq_cos_angle) > 1.0) {
            sq_cos_angle = 1;
        }
        double sin_angle = sqrt(1 - sq_cos_angle);
        area += sin_angle / 2 * sqrt(a * b);
    }
    return area;
}

// precision problems may occur when comparing doubles with 0
bool check_for_zero(rectangle rect) {
    // precision for comparing
    double comparing_precision = 1e-5;
    //cout << count_area(make_pair(0.0, 0.0), rect) << " " << count_area(rect[0], rect) << endl;
    return comparing_precision > count_area(make_pair(0.0, 0.0), rect) - count_area(rect[0], rect);
}

int main() {
    size_t n, m;
    cin >> n;
    rectangle rect1(n);
    for (size_t i = 0; i < n; ++i) {
        cin >> rect1[i].first >> rect1[i].second;
    }

    cin >> m;
    rectangle rect2(m);
    for (size_t i = 0; i < m; ++i) {
        cin >> rect2[i].first >> rect2[i].second;
    }

    //cout << count_area(rect1[0], rect1) << " " << count_area(rect2[0], rect2) << endl;
    rectangle temp = reflected_vector(rect2);
    if(check_for_zero(minkovsky(rect1, temp))) {
        cout << "YES";
    }
    else {
        cout << "NO";
    }
    return 0;
}
