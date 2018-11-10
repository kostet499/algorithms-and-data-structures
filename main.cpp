#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

using std::vector;
using std::cout;
using std::cin;
using std::endl;

struct point {
    double x;
    double y;
    double z;

    point(double v1, double v2, double v3) {
        x = v1;
        y = v2;
        z = v3;
    }

    void swap(point&, point&);
};

void point::swap(point &a, point &b) {

}

class ConvexHull {
private:

private:

public:
    ConvexHull(const vector<point> &point_set) {
        /*
        std::random_device rd;
        std::mt19937 generator(rd());
        std::shuffle(point_set.begin(), point_set.end(), generator);
        */
    }
};


int main() {
    int test_number;
    cin >> test_number;
    for(int i = 0; i < test_number; ++i) {
        int points_number;
        cin >> points_number;

        for(int j = 0; j < points_number; ++j) {

        }
    }
    return 0;
}