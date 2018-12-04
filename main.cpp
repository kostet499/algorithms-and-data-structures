#include <iostream>
#include <unordered_map>
#include <set>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <utility>

using namespace std;

using pos = pair<unsigned short, unsigned short>;

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
        storage.resize(graph.width());
        for(auto &row : storage) {
            row.resize(graph.height());
        }

        iterateOverField(RestoreField);
    }

    void iterateOverField(bool (*function)(size_t, size_t, GameGraph &graph, vector<vector<short> > &storage)) {
        for(size_t i = 0; i < graph.width(); ++i) {
            for(size_t j = 0; j < graph.height(); ++j) {
                if(!function(i, j, graph, storage)) {
                    return;
                }
            }
        }
    }

    static bool RestoreField(size_t i, size_t j, GameGraph &graph, vector<vector<short> > &storage) {
        pos curr(i, j);
        field(curr, storage) = 0;
        const pos &ferz = graph.repr().ferz;
        const pos &king = graph.repr().king;
        int dif_fst = abs(ferz.first - curr.first);
        int dif_scd = abs(ferz.second - curr.second);
        if(GameGraph::Manhattan(curr, graph.wk()) == 1) {
            field(curr, storage) = 1;
        }
        else if(curr == graph.wk()) {
            field(curr, storage) = 2;
        }
        else if(curr == ferz) {
            field(curr, storage) = 3;
        }
        else if((dif_fst == 0 || dif_scd == 0 || dif_fst == dif_scd) &&
        !(dif_fst == abs(ferz.first - king.first) + abs(king.first - curr.first) &&
        dif_scd == abs(ferz.second - king.second) + abs(king.second - curr.second)) ) {
            field(curr, storage) = 1;
        }
        return true;
    }

private:
    static short &field(const pos &position, vector<vector<short> > &storage) {
        return storage[position.first][position.second];
    }


private:
    vector<vector<short> > storage;
    GameGraph &graph;
};

int main() {
    data representer(make_pair(0, 0), make_pair(0, 0), 0);
    GameGraph mygraph(representer);
    Game mygame(mygraph);

    return 0;
}