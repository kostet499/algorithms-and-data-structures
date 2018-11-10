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
        first_initialization(point_set);
    }

private:

    // -1 - not seen, 0 - in same plane, 1 - seen
    short int is_seen(size_t facet_id, size_t point_id, const vector<point> &point_set) const {
        point normal = outside_normal(facet_id, point_set);
        point vector_to_point = compute_vector(point_set[facet[facet_id][0]], point_set[point_id]);

        double comparing_precision = 1e-10;
        double cos_angle = compute_cos_angle(vector_to_point, normal);
        if(std::fabs(cos_angle) < comparing_precision) {
            return 0;
        }
        else if(cos_angle < 0) {
            return -1;
        }
        else {
            return 1;
        }
    }

    point outside_normal(size_t facet_id, const vector<point> &point_set) const {
        point a = point_set[facet[facet_id][0]];
        point b = point_set[facet[facet_id][1]];
        point c = point_set[facet[facet_id][2]];
        point d = point_set[not_on_facet[facet_id]];

        point ab = compute_vector(a, b);
        point ac = compute_vector(a, c);
        point ad = compute_vector(a, d);

        point normal = compute_vector_multiply(ab, ac);
        if(compute_cos_angle(ad, normal) > 0) {
            point zero_point = point(0, 0, 0);
            // normal = (-normal.x, -normal.y, -normal.z)
            normal = compute_vector(normal, zero_point);
        }

        return normal;
    }

    // we hardcode smth
    void first_initialization(const vector<point> &point_set) {
        facet.emplace_back(vector<size_t>({0, 1, 2}));
        not_on_facet.emplace_back(3);
        facet.emplace_back(vector<size_t>({0, 1, 3}));
        not_on_facet.emplace_back(2);
        facet.emplace_back(vector<size_t>({0, 2, 3}));
        not_on_facet.emplace_back(1);
        facet.emplace_back(vector<size_t>({1, 2, 3}));
        not_on_facet.emplace_back(0);


    }

    point compute_vector(const point &a, const point &b) const {
        return point(b.x - a.x, b.y - a.y, b.z - a.z);
    }

    point compute_vector_multiply(const point &a, const point &b) const {
        return point(a.y * b.z - a.z * b.y, -a.x * b.z + a.z * b.x, a.x * b.y - a.y * b.x);
    }

    double compute_vector_norm(const point &a) const {
        return sqrt(pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2));
    }

    double compute_vector_scalar(const point &a, const point &b) const {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    double compute_cos_angle(const point &a, const point &b) const {
        return compute_vector_scalar(a, b) / compute_vector_norm(a) / compute_vector_norm(b);
    }

private:
    vector<vector<size_t> > facet;
    vector<vector<size_t> > point_conflict;
    vector<vector<size_t> > facet_conflict;
    vector<size_t> not_on_facet;
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