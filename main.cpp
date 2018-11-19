#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>
#include <algorithm>

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
        for(point_index = 4; point_index < point_set.size(); ++point_index) {
            add_point(point_set);
        }
    }

    void represent_answer() {
        facet.sort();
        std::cout << facet.size() << std::endl;
        for(auto &mface : facet) {
            std::cout << 3 << " " << mface.first[0] << " " << mface.first[1] << " " << mface.first[2] << std::endl;
        }
    }
private:
    void sort_facet(vector<size_t> &mface, const vector<point> &point_set) {
        std::sort(mface.begin(), mface.end());
        point ab = point_set[mface[1]] - point_set[mface[0]];
        point ac = point_set[mface[2]] - point_set[mface[0]];
        if(outside_normal(mface, point_set)(ab[ac]) < 0) {
            std::swap(mface[1], mface[2]);
        }
    }

    bool is_seen(vector<size_t> &mface, size_t point_id, const vector<point> &point_set) const {
        return (point_set[point_id] - point_set[mface[0]])(outside_normal(mface, point_set)) > 0;
    }

    point outside_normal(vector<size_t> &mface, const vector<point> &point_set) const {
        point ab = point_set[mface[1]] - point_set[mface[0]];
        point ac = point_set[mface[2]] - point_set[mface[0]];
        return (super_point - point_set[mface[0]])(ab[ac]) > 0 ? ac[ab] : ab[ac];
    }

    void add_point(const vector<point> &point_set) {
        std::unordered_map<size_t, bool> facet_seen;
        for(auto mface = facet.begin(); mface != facet.end(); ++mface) {
            facet_seen[mface->second] = is_seen(mface->first, point_index, point_set);
            if(facet_seen[mface->second]) {
                mface = facet.erase(mface);
                --mface;
            }
        }

        std::unordered_map<size_t, size_t> temp;
        std::list<vector<size_t> > new_edges;
        for(auto medge = edges.begin(); medge != edges.end(); ++medge) {
            size_t facet_id1 = (*medge)[2];
            size_t facet_id2 = (*medge)[3];
            if(facet_seen[facet_id1] && facet_seen[facet_id2]) {
                medge = edges.erase(medge);
                --medge;
            }
            else if(facet_seen[facet_id1] || facet_seen[facet_id2]) {
                size_t new_facet_id = hardcode_facet((*medge)[0], (*medge)[1], point_index, point_set);
                (*medge)[2] = new_facet_id;
                (*medge)[3] = facet_seen[facet_id2] ? facet_id1 : facet_id2;
                for(size_t j = 0; j < 2; ++j) {
                    if (temp.find(encode(point_index, (*medge)[j])) != temp.end()) {
                        edges.emplace_front(vector<size_t>(
                                {point_index, (*medge)[j], temp[encode(point_index, (*medge)[j])], new_facet_id}));
                    } else {
                        temp[encode(point_index, (*medge)[j])] = new_facet_id;
                    }
                }
            }
        }
    }

    void first_initialization(const vector<point> &point_set) {
        super_point = (point_set[0] + point_set[1] + point_set[2] + point_set[3]) * 0.25;

        hardcode_facet(0, 1, 2, point_set); // facet 0
        hardcode_facet(3, 0, 1, point_set); // facet 1
        hardcode_facet(2, 3, 0, point_set); // facet 2
        hardcode_facet(1, 2, 3, point_set); // facet 3

        edges.emplace_back(vector<size_t>({0, 1, 0, 1}));
        edges.emplace_back(vector<size_t>({3, 0, 1, 2}));
        edges.emplace_back(vector<size_t>({2, 3, 2, 3}));
        edges.emplace_back(vector<size_t>({1, 2, 3, 0}));
        edges.emplace_back(vector<size_t>({0, 2, 0, 2}));
        edges.emplace_back(vector<size_t>({3, 1, 3, 1}));
    }

    size_t hardcode_facet(size_t id1, size_t id2, size_t id3, const vector<point> &point_set) {
        facet.emplace_back(std::make_pair(vector<size_t>({id1, id2, id3}), facet_index));
        sort_facet(facet.back().first, point_set);
        return facet_index++;
    }

    size_t encode(size_t a, size_t b) const {
        if(a > b) {
            std::swap(a, b);
        }
        return (a << 15) + b;
    }

private:
    std::list<std::pair<vector<size_t>, size_t> > facet;
    std::list<vector<size_t> > edges;
    point super_point;
    size_t point_index = 0, facet_index = 0;
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