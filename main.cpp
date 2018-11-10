#include <iostream>
#include <vector>
#include <algorithm>

using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::random_shuffle;

struct point {
    double x;
    double y;
    double z;

    point(double v1, double v2, double v3) {
        x = v1;
        y = v2;
        z = v3;
    }
};

class ConvexHull {
private:

private:

public:
    ConvexHull(const vector<point> &point_set) {
        random_shuffle(point_set.begin(), point_set.end());
    }
};


int main() {
    int test_number;
    cin >> test_number;
    for(int i = 0; i < test_number; ++i) {
        int points_number;
        cin >> points_number;

        for(int i = 0; i < points_number; ++i) {

        }
    }
    return 0;
}