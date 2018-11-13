#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <unordered_map>
#include <queue>

using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::queue;
using std::unordered_map;

struct point {
    double x, y, z;
    point(double v1, double v2, double v3) : x(v1), y(v2), z(v3){}
};

class ConvexHull {
public:
    ConvexHull(const vector<point> &point_set) : super_point(0, 0, 0){
        first_initialization(point_set);
        for(curr_index = 4; curr_index < point_set.size(); ++curr_index) {
            add_point(point_set);
        }
    }

    void represent_answer() {
        // experimental sort
        std::sort(facet.begin(), facet.end());
        std::cout << facet.size() - free_facets.size() << endl;
        for(size_t i = 0; i < facet.size(); ++i) {
            if(facet[i].empty()) {
                continue;
            }
            cout << 3 << " " << facet[i][0] << " " << facet[i][1] << " " << facet[i][2] << endl;
        }
    }
private:
    // hard thing
    void sort_facet(size_t facet_id, const vector<point> &point_set) {
        std::sort(facet[facet_id].begin(), facet[facet_id].end());
        point normal = outside_normal(facet_id, point_set);
        point ab = compute_vector(point_set[facet[facet_id][0]], point_set[facet[facet_id][1]]);
        point ac = compute_vector(point_set[facet[facet_id][0]], point_set[facet[facet_id][2]]);
        point mult = compute_vector_multiply(ab, ac);
        if(compute_vector_scalar(normal, mult) < 0) {
            std::swap(facet[facet_id][1], facet[facet_id][2]);
        }
    }

    // bool - only for this task - we have no complanar 4 points
    bool is_seen(size_t facet_id, size_t point_id, const vector<point> &point_set) const {
        point normal = outside_normal(facet_id, point_set);
        point vector_to_point = compute_vector(point_set[facet[facet_id][0]], point_set[point_id]);
        return compute_vector_scalar(vector_to_point, normal) > 0;
    }

    point outside_normal(size_t facet_id, const vector<point> &point_set) const {
        point ab = compute_vector(point_set[facet[facet_id][0]], point_set[facet[facet_id][1]]);
        point ac = compute_vector(point_set[facet[facet_id][0]], point_set[facet[facet_id][2]]);
        point ad = compute_vector(point_set[facet[facet_id][0]], super_point);

        point normal = compute_vector_multiply(ab, ac);
        if(compute_vector_scalar(ad, normal) > 0) {
            return compute_vector_number(normal, -1);
        }
        return normal;
    }

    // developing
    void add_point(const vector<point> &point_set) {
        vector<bool> facet_seen(facet.size(), false);
        for(size_t i = 0; i < facet.size(); ++i) {
            if(facet[i].empty()) {
                continue;
            }
            facet_seen[i] = is_seen(i, curr_index, point_set);
            if(facet_seen[i]) {
                facet[i].clear();
                free_facets.push(i);
            }
        }

        // considers 3 cases
        // edge fully seen then deleted
        // edge fully unseen then nothing
        // edge partially seen then it is on the horizon
        unordered_map<size_t, size_t> temp;
        vector<vector<size_t> > new_edges;
        for(size_t i = 0; i < edges.size(); ++i) {
            if(edges[i].empty()) {
                continue;
            }
            size_t facet_id1 = edges[i][2];
            size_t facet_id2 = edges[i][3];
            if(facet_seen[facet_id1] && facet_seen[facet_id2]) {
                edges[i].clear();
                free_edges.push(i);
            }
            else if(!facet_seen[facet_id1] && !facet_seen[facet_id2]) {
                continue;
            }
            else {
                edges[i].clear();
                free_edges.push(i);
                size_t new_facet_id = hardcode_facet(edges[i][0], edges[i][1], curr_index, point_set);
                // experimental work with unordered map
                if(temp.find(encode(curr_index, edges[i][0])) != temp.end()) {
                    new_edges.emplace_back(vector<size_t>({curr_index, edges[i][0], temp[encode(curr_index, edges[i][0])], new_facet_id}));
                }
                else {
                    temp[encode(curr_index, edges[i][0])] = new_facet_id;
                }

                if(temp.find(encode(curr_index, edges[i][1])) != temp.end()) {
                    new_edges.emplace_back(vector<size_t>({curr_index, edges[i][1], temp[encode(curr_index, edges[i][1])], new_facet_id}));
                }
                else {
                    temp[encode(curr_index, edges[i][1])] = new_facet_id;
                }
            }

        }

        for(const auto &vec : new_edges) {
            hardcode_edge(vec[0], vec[1], vec[2], vec[3]);
        }
    }

    // we hardcode smth
    void first_initialization(const vector<point> &point_set) {
        // experimetal
        point a = compute_vector_number(point_set[0], 1.0 / 4);
        point b = compute_vector_number(point_set[1], 1.0 / 4);
        point c = compute_vector_number(point_set[2], 1.0 / 4);
        point d = compute_vector_number(point_set[3], 1.0 / 4);
        super_point = compute_vector_sum(a, b);
        super_point = compute_vector_sum(super_point, c);
        super_point = compute_vector_sum(super_point, d);

        hardcode_facet(0, 1, 2, point_set); // facet 0
        hardcode_facet(3, 0, 1, point_set); // facet 1
        hardcode_facet(2, 3, 0, point_set); // facet 2
        hardcode_facet(1, 2, 3, point_set); // facet 3

        hardcode_edge(0, 1, 0, 1);
        hardcode_edge(3, 0, 1, 2);
        hardcode_edge(2, 3, 2, 3);
        hardcode_edge(1, 2, 3, 0);
        hardcode_edge(0, 2, 0, 2);
        hardcode_edge(3, 1, 3, 1);
    }

    size_t hardcode_facet(size_t id1, size_t id2, size_t id3, const vector<point> &point_set) {
        if(free_facets.empty()) {
            facet.emplace_back(vector<size_t>({id1, id2, id3}));
            sort_facet(facet.size() - 1, point_set);
            return facet.size() - 1;
        }
        size_t free_place = free_facets.front();
        free_facets.pop();
        facet[free_place] = {id1, id2, id3};
        sort_facet(free_place, point_set);
        return free_place;
    }

    void hardcode_edge(size_t id1, size_t id2, size_t facet_id1, size_t facet_id2) {
        if(free_edges.empty()) {
            edges.emplace_back(vector<size_t>({id1, id2, facet_id1, facet_id2}));
            return;
        }
        size_t free_place = free_edges.front();
        free_edges.pop();
        edges[free_place] = {id1, id2, facet_id1, facet_id2};
    }

    size_t encode(size_t a, size_t b) const {
        if(a > b) {
            std::swap(a, b);
        }
        return (a << 15) + b;
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

    double compute_vector_scalar(const point &a, const point &b) const {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

private:
    vector<vector<size_t> > facet;
    size_t curr_index;
    vector<vector<size_t> > edges;
    queue<size_t> free_facets;
    queue<size_t> free_edges;
    // supposed to be a point inside the convex hull
    point super_point;
};


int main() {
    int test_number;
    cin >> test_number;
    for(int i = 0; i < test_number; ++i) {
        int points_number;
        cin >> points_number;
        vector<point> point_set;
        for(int j = 0; j < points_number; ++j) {
            int x, y, z;
            cin >> x >> y >> z;
            point_set.emplace_back(point(x, y, z));
        }
        ConvexHull myhull(point_set);
        myhull.represent_answer();
    }
    return 0;
}