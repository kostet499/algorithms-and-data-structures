#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <unordered_map>

using std::unordered_map;
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
    ConvexHull(const vector<point> &point_set) : super_point(0, 0, 0){
        /*
        std::random_device rd;
        std::mt19937 generator(rd());
        std::shuffle(point_set.begin(), point_set.end(), generator);
        */
        first_initialization(point_set);
        for(curr_index = 4; curr_index < point_set.size(); ++curr_index) {
            add_point(point_set);
        }
    }

private:

    // bool - only for this task - we have no complanar 4 points
    short int is_seen(size_t facet_id, size_t point_id, const vector<point> &point_set) const {
        point normal = outside_normal(facet_id, point_set);
        point vector_to_point = compute_vector(point_set[facet[facet_id][0]], point_set[point_id]);
        return compute_cos_angle(vector_to_point, normal) > 0;
    }

    point outside_normal(size_t facet_id, const vector<point> &point_set) const {
        point ab = compute_vector(point_set[facet[facet_id][0]], point_set[facet[facet_id][1]]);
        point ac = compute_vector(point_set[facet[facet_id][0]], point_set[facet[facet_id][2]]);
        point ad = compute_vector(point_set[facet[facet_id][0]], super_point);

        point normal = compute_vector_multiply(ab, ac);
        if(compute_cos_angle(ad, normal) > 0) {
            normal = compute_vector_number(normal, -1);
        }

        return normal;
    }

    void add_point(const vector<point> &point_set) {
        vector<bool> facet_seen(facet.size(), false);
        for(size_t i = 0; i < facet.size(); ++i) {
            facet_seen[i] = is_seen(i, curr_index, point_set);
        }

    }

    // we hardcode smth
    void first_initialization(const vector<point> &point_set) {
        // experimetal
        point a = compute_vector_number(point_set[0], 1/3);
        point b = compute_vector_number(point_set[1], 1/3);
        point c = compute_vector_number(point_set[2], 1/3);
        point d = compute_vector_number(point_set[3], 1/2);
        super_point = compute_vector_sum(a, b);
        super_point = compute_vector_sum(super_point, c);
        super_point = compute_vector_sum(super_point, d);

        hardcode_facet(0, 1, 2); // facet 0
        hardcode_facet(3, 0, 1); // facet 1
        hardcode_facet(2, 3, 0); // facet 2
        hardcode_facet(1, 2, 3); // facet 3

        hardcode_edge(0, 1, 0, 1);
        hardcode_edge(3, 0, 1, 2);
        hardcode_edge(2, 3, 2, 3);
        hardcode_edge(1, 2, 3, 0);
        hardcode_edge(0, 2, 0, 2);
        hardcode_edge(3, 1, 3, 1);
    }

    void hardcode_facet(size_t id1, size_t id2, size_t id3) {
        facet.emplace_back(vector<size_t>({id1, id2, id3}));
    }

    void hardcode_edge(size_t id1, size_t id2, size_t facet_id1, size_t facet_id2) {
        edges[encode(id1, id2)] = encode(facet_id1, facet_id2);
    }

    size_t encode(size_t a, size_t b) const {
        if(a > b) {
            std::swap(a, b);
        }
        return (a << 15) + b;
    }

    std::pair<size_t, size_t> decode(size_t encoded_value)  const {
        return std::make_pair(encoded_value >> 15, encoded_value & ((1 << 15) - 1));
    }

    point compute_vector(const point &a, const point &b) const {
        return {b.x - a.x, b.y - a.y, b.z - a.z};
    }

    point compute_vector_sum(const point &a, const point &b) const {
        return {b.x + a.x, b.y + a.y, b.z + a.z};
    }

    point compute_vector_multiply(const point &a, const point &b) const {
        return {a.y * b.z - a.z * b.y, -a.x * b.z + a.z * b.x, a.x * b.y - a.y * b.x};
    }

    point compute_vector_number(const point &a, double number) const {
        return {a.x * number, a.y * number, a.z * number};
    }
    double compute_vector_norm(const point &a) const {
        return sqrt(pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2));
    }

    double compute_vector_scalar(const point &a, const point &b) const {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    double compute_cos_angle(const point &a, const point &b) const {
        double norma = compute_vector_norm(a);
        double normb = compute_vector_norm(b);
        // problem oriented condition
        if(norma < comparing_precision || normb < comparing_precision) {
            return 0;
        }
        return compute_vector_scalar(a, b) / norma / normb;
    }

private:
    vector<vector<size_t> > facet;
    size_t curr_index;
    double comparing_precision = 1e-10;
    unordered_map<size_t, size_t> edges;
    // supposed to be a point inside the convex hull
    point super_point;
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