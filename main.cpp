#include <iostream>
#include <unordered_map>
#include <set>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <stack>

using namespace std;

using pos = pair<short, short>;

struct data {
    pos king;
    pos ferz;
    bool king_turn;

    data(pos a, pos b, bool d) : king(std::move(a)), ferz(std::move(b)), king_turn(d) {
    }

    using val_t = unsigned long long;
    val_t hash() const {
        return static_cast<val_t>(king_turn) +
                (static_cast<val_t>(king.first) << 1) +
                (static_cast<val_t>(king.second) << 9) +
                (static_cast<val_t>(ferz.first) << 17) +
                (static_cast<val_t>(ferz.second) << 25);
    }
};

class GameGraph {
public:
    explicit GameGraph(data &fst) : representer(fst){
        width_ = 8;
        height_ = 8;
        king = make_pair(2, 2);
        for(size_t i = 0; i < width_; ++i) {
            for(size_t j = 0; j < height_; ++j) {
                for(size_t k = 0; k < width_; ++k) {
                    for(size_t h = 0; h < height_; ++h) {
                        for(size_t p = 0; p < 2; ++p) {
                            data to_insert = data(make_pair(i, j), make_pair(k, h),static_cast<bool>(p));
                            if(IsRightData(to_insert)) {
                                storage[to_insert.hash()] = -1;
                                iterated.emplace_back(to_insert);
                            }
                        }
                    }
                }
            }
        }
    }

    size_t width() const {
        return width_;
    }

    size_t height() const {
        return height_;
    }

    short&operator[](const data &i) {
        return storage[i.hash()];
    }

    const data&operator[](int i) {
        return iterated[i];
    }

    const pos& wk() {
        return king;
    }

    const size_t size() {
        return iterated.size();
    }

    bool IsRightData(const data &sit) const {
        if(sit.king == sit.ferz || king == sit.ferz || Manhattan(king, sit.king) < 2) {
            return false;
        }

        return IsRightPos(sit.king) && IsRightPos(sit.ferz);
    }

    static int Manhattan(const pos &a, const pos &b) {
        return abs(a.first - b.first) + abs(a.second - b.second);
    }

    bool IsRightPos(const pos &a) const {
        return !(a.first < 0 || a.second < 0 || a.first >= width_ || a.second >= height_);
    }

    data &repr() {
        return representer;
    }
private:
    data &representer;
    unordered_map<unsigned long long, short> storage;
    std::vector<data> iterated;
    pos king;
    size_t width_;
    size_t height_;
};

class Game {
public:
    explicit Game(GameGraph &gamegraph) : graph(gamegraph) {
        initialize();
    }

    int GetState(const pos &check) const {
        const pos &ferz = graph.repr().ferz;
        const pos &king = graph.repr().king;
        int dif_fst = abs(ferz.first - check.first);
        int dif_scd = abs(ferz.second - check.second);
        if(GameGraph::Manhattan(check, graph.wk()) == 1) {
            return 1;
        }
        if(check == graph.wk()) {
            return 2;
        }
        if(check == ferz) {
            return 3;
        }
        if((dif_fst == 0 || dif_scd == 0 || dif_fst == dif_scd) &&
                !(dif_fst == abs(ferz.first - king.first) + abs(king.first - check.first) &&
                  dif_scd == abs(ferz.second - king.second) + abs(king.second - check.second)) ) {
            return 1;
        }
        return 0;
    }

    bool NoStep(const pos &check) const {
        for(int i = -1; i < 2; ++i) {
            for(int j = -1; j < 2; ++j) {
                if(j == 0 && i == 0) {
                    continue;
                }
                pos near_to_check(check.first + i, check.second + j);
                if(graph.IsRightPos(near_to_check) && storage[near_to_check.first][near_to_check.second] != 1) {
                    return false;
                }
            }
        }
        return true;
    }

    bool IsPat(const data &check);

private:
    void initialize() {
        for (size_t i = 0; i < graph.size(); ++i) {

        }
    }

private:
    GameGraph &graph;
    stack<data> new_sits;
};

int main() {
    data representer(make_pair(0, 0), make_pair(0, 0), false);
    GameGraph mygraph(representer);
    Game mygame(mygraph);

    return 0;
}