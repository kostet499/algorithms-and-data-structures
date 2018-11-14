#include <algorithm>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>

using std::vector;

struct point {
    double x, y, z;
    point(double v1, double v2, double v3) : x(v1), y(v2), z(v3){}
    point operator+(const point &b) const {
        return {this->x + b.x, this->y + b.y, this->z + b.z};
    }
    point operator*(double number) const {
        return {this->x * number, this->y * number, this->z * number};
    }
    point operator[](const point &b) const {
        return {this->y * b.z - this->z * b.y, -this->x * b.z + this->z * b.x, this->x * b.y - this->y * b.x};
    }
    point operator-(const point &b) const {
        return {this->x - b.x, this->y - b.y, this->z - b.z};
    }
    double operator()(const point &b) const {
        return this->x * b.x + this->y * b.y + this->z * b.z;
    }
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
        std::sort(facet.begin(), facet.end());
        std::cout << facet.size() - free_facets.size() << std::endl;
        for(size_t i = 0; i < facet.size(); ++i) {
            if(facet[i].empty()) {
                continue;
            }
            std::cout << 3 << " " << facet[i][0] << " " << facet[i][1] << " " << facet[i][2] << std::endl;
        }
    }
private:
    void sort_facet(size_t facet_id, const vector<point> &point_set) {
        std::sort(facet[facet_id].begin(), facet[facet_id].end());
        point ab = point_set[facet[facet_id][1]] - point_set[facet[facet_id][0]];
        point ac = point_set[facet[facet_id][2]] - point_set[facet[facet_id][0]];
        if(outside_normal(facet_id, point_set)(ab[ac]) < 0) {
            std::swap(facet[facet_id][1], facet[facet_id][2]);
        }
    }

    bool is_seen(size_t facet_id, size_t point_id, const vector<point> &point_set) const {
        return (point_set[point_id] - point_set[facet[facet_id][0]])(outside_normal(facet_id, point_set)) > 0;
    }

    point outside_normal(size_t facet_id, const vector<point> &point_set) const {
        point ab = point_set[facet[facet_id][1]] - point_set[facet[facet_id][0]];
        point ac = point_set[facet[facet_id][2]] - point_set[facet[facet_id][0]];
        return (super_point - point_set[facet[facet_id][0]])(ab[ac]) > 0 ? ac[ab] : ab[ac];
    }

    void add_point(const vector<point> &point_set) {
        vector<bool> facet_seen(facet.size(), false);
        for(size_t i = 0; i < facet.size(); ++i) {
            if(!facet[i].empty() && (facet_seen[i] = is_seen(i, curr_index, point_set))) {
                facet[i].clear();
                free_facets.push(i);
            }
        }

        std::unordered_map<size_t, size_t> temp;
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
            else if(facet_seen[facet_id1] || facet_seen[facet_id2]) {
                size_t new_facet_id = hardcode_facet(edges[i][0], edges[i][1], curr_index, point_set);
                edges[i][2] = new_facet_id;
                edges[i][3] = facet_seen[facet_id2] ? facet_id1 : facet_id2;
                for(size_t j = 0; j < 2; ++j) {
                    if (temp.find(encode(curr_index, edges[i][j])) != temp.end()) {
                        new_edges.emplace_back(vector<size_t>(
                                {curr_index, edges[i][j], temp[encode(curr_index, edges[i][j])], new_facet_id}));
                    } else {
                        temp[encode(curr_index, edges[i][j])] = new_facet_id;
                    }
                }
            }

        }

        for(const auto &vec : new_edges) {
            hardcode_edge(vec[0], vec[1], vec[2], vec[3]);
        }
    }

    void first_initialization(const vector<point> &point_set) {
        super_point = (point_set[0] + point_set[1] + point_set[2] + point_set[3]) * 0.25;

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

private:
    vector<vector<size_t> > facet, edges;
    std::queue<size_t> free_facets, free_edges;
    point super_point;
    size_t curr_index;
};

int main() {
    int test_number;
    std::cin >> test_number;
    for(int i = 0; i < test_number; ++i) {
        int points_number;
        std::cin >> points_number;
        vector<point> point_set;
        for(int j = 0; j < points_number; ++j) {
            int x, y, z;
            std::cin >> x >> y >> z;
            point_set.emplace_back(point(x, y, z));
        }
        ConvexHull myhull(point_set);
        myhull.represent_answer();
    }
    return 0;
}