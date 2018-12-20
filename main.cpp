#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>
#include <algorithm>

using std::vector;
class Point {
public:
    Point(double v1, double v2, double v3) : x(v1), y(v2), z(v3){}
    Point operator+(const Point &b) const {
        return {this->x + b.x, this->y + b.y, this->z + b.z};
    }
    Point operator*(double number) const {
        return {this->x * number, this->y * number, this->z * number};
    }
    Point operator[](const Point &b) const {
        return {this->y * b.z - this->z * b.y, -this->x * b.z + this->z * b.x, this->x * b.y - this->y * b.x};
    }
    Point operator-(const Point &b) const {
        return {this->x - b.x, this->y - b.y, this->z - b.z};
    }
    double operator()(const Point &b) const {
        return this->x * b.x + this->y * b.y + this->z * b.z;
    }
private:
    double x, y, z;
};

class Facet {
public:
    Facet(size_t val1, size_t val2, size_t val3) {
        face = {val1, val2, val3};
    }

    const size_t operator[](size_t index) const {
        return face[index];
    }

    vector<size_t>& operator()() {
        return face;
    }

    bool operator<(const Facet &other) {
        return face < other.face;
    }
private:
    vector<size_t> face;
};

class ConvexHull {
public:
    ConvexHull(const vector<Point> &point_set) : super_point(0, 0, 0){
        first_initialization(point_set);
        for(point_index = 4; point_index < point_set.size(); ++point_index) {
            add_point(point_set);
        }
    }

    void represent_answer() {
        facelist.sort();
        std::cout << facelist.size() << std::endl;
        for(const auto &mface : facelist) {
            std::cout << 3 << " " << mface[0] << " " << mface[1] << " " << mface[2] << std::endl;
        }
    }
private:
    void sort_facet(Facet &mface, const vector<Point> &point_set) {
        std::sort(mface().begin(), mface().end());
        Point ab = point_set[mface[1]] - point_set[mface[0]];
        Point ac = point_set[mface[2]] - point_set[mface[0]];
        if(outside_normal(mface, point_set)(ab[ac]) < 0) {
            std::swap(mface()[1], mface()[2]);
        }
    }

    bool is_seen(const Facet &mface, size_t point_id, const vector<Point> &point_set) const {
        return (point_set[point_id] - point_set[mface[0]])(outside_normal(mface, point_set)) > 0;
    }

    Point outside_normal(const Facet &mface, const vector<Point> &point_set) const {
        Point ab = point_set[mface[1]] - point_set[mface[0]];
        Point ac = point_set[mface[2]] - point_set[mface[0]];
        return (super_point - point_set[mface[0]])(ab[ac]) > 0 ? ac[ab] : ab[ac];
    }

    void add_point(const vector<Point> &point_set) {
        std::unordered_map<size_t, bool> edges;
        for(auto iter = facelist.begin(); iter != facelist.end(); ++iter) {
            if(is_seen(*iter, point_index, point_set)) {
                for(size_t i = 0; i < 3; ++i) {
                    size_t cypher = encode((*iter)[i], (*iter)[(i + 1) % 3]);
                    if(edges.find(cypher) == edges.end()) {
                        edges[cypher] = true;
                    }
                    else {
                        edges.erase(cypher);
                    }
                }
                iter = facelist.erase(iter);
                --iter;
            }
        }

        for(auto iter : edges) {
            size_t val1 = iter.first >> 15;
            size_t val2 = iter.first - (val1 << 15);
            facelist.emplace_back(val1, val2, point_index);
            sort_facet(facelist.back(), point_set);
        }
    }

    size_t encode(size_t val1, size_t val2) const {
        if(val1 > val2) {
            std::swap(val1, val2);
        }
        return (val1 << 15) + val2;
    }

    void first_initialization(const vector<Point> &point_set) {
        super_point = (point_set[0] + point_set[1] + point_set[2] + point_set[3]) * 0.25;
        for(size_t i = 0; i < 4; ++i) {
            facelist.emplace_back(i, (i + 1) % 4, (i + 2) % 4);
            sort_facet(facelist.back(), point_set);
        }
    }

private:
    std::list<Facet> facelist;
    Point super_point;
    size_t point_index = 0;
};

int main() {
    int test_number;
    std::cin >> test_number;
    for(int i = 0; i < test_number; ++i) {
        int points_number;
        std::cin >> points_number;
        vector<Point> point_set;
        for(int j = 0; j < points_number; ++j) {
            int x, y, z;
            std::cin >> x >> y >> z;
            point_set.emplace_back(Point(x, y, z));
        }
        ConvexHull myhull(point_set);
        myhull.represent_answer();
    }
    return 0;
}