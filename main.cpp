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
public:
    ConvexHull(const vector<point> &point_set) {
        /*
        std::random_device rd;
        std::mt19937 generator(rd());
        std::shuffle(point_set.begin(), point_set.end(), generator);
        */
        first_initialization();
    }

private:
    // we hardcode smth
    void first_initialization() {
        facet.emplace_back(vector<size_t>({0, 1, 2}));
        facet.emplace_back(vector<size_t>({0, 1, 4}));
        facet.emplace_back(vector<size_t>({1, 2, 3}));
        facet.emplace_back(vector<size_t>({1, 2, 4}));


    }

    point compute_vector(point &a, point &b) {
        return point(b.x - a.x, b.y - a.y, b.z - a.z);
    }

    point compute_vector_multiply(point &a, point &b) {
        return point(a.y * b.z - a.z * b.y, -a.x * b.z + a.z * b.x, a.x * b.y - a.y * b.x);
    }

    double compute_vector_sq_norm(point &a) {
        return pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2);
    }

    double compute_vector_scalar(point &a, point &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

private:
    vector<vector<size_t> > facet;
    vector<vector<size_t> > point_conflict;
    vector<vector<size_t> > facet_conflict;
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