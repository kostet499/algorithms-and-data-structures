#include <iostream>
#include <unordered_set>
#include <set>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <utility>

using namespace std;

using pos = pair<short, short>;

struct data {
    pos king;
    pos ferz;
    size_t step;
    bool king_turn;

    data(pos a, pos b, size_t c, bool d) : king(std::move(a)), ferz(std::move(b)), step(c), king_turn(d) {
    }
};

class GameGraph {
public:
    GameGraph() {
        width_ = 8;
        height_ = 8;
        king = make_pair(2, 2);
        for(size_t i = 0; i < width_; ++i) {
            for(size_t j = 0; j < height_; ++j) {
                for(size_t k = 0; k < width_; ++k) {
                    for(size_t h = 0; h < height_; ++h) {
                        for(size_t p = 0; p < 2; ++p) {
                            storage.emplace_back(make_pair(i, j), make_pair(k, h), 0, static_cast<bool>(p));
                            if(IsRightData(storage.back()))
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

    data&operator[](int i) {
        return storage[i];
    }

    const pos& wk() {
        return king;
    }

    bool IsRightData(const data &sit) const {
        if(sit.king == sit.ferz || king == sit.ferz || Manhattan(king, sit.king) < 2) {
            return false;
        }

        if(!IsRightPos(sit.king) || !IsRightPos(sit.ferz)) {
            return false;
        }
    }

    static int Manhattan(const pos &a, const pos &b) {
        return abs(a.first - b.first) + abs(a.second - b.second);
    }

    bool IsRightPos(const pos &a) const {
        return !(a.first < 0 || a.second < 0 || a.first >= width_ || a.second >= height_);
    }
private:
    vector<data> storage;
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
private:
    void iterateOverField(bool (*function)(size_t, size_t)) {
        for(size_t i = 0; i < graph.width(); ++i) {
            for(size_t j = 0; j < graph.height(); ++j) {
                if(!function(i, j)) {
                    return;
                }
            }
        }
    }

    void FillDataset(const data &pst) {
        vector<pos> possible_poses;
        for(int i = -1; i < 2; ++i) {
            for(int j = -1; j < 2; ++j) {
                if(i != 0 || j != 0) {
                    FillFiller(pst.ferz, {i, j}, possible_poses);
                }
            }
        }
        for(auto &poset : possible_poses) {
            data new_data = pst;
            ++new_data.step;
            new_data.ferz = poset;
            if(new_data.step < global_answer) {
                dataset.insert(new_data);
            }
            --new_data.step;
        }
    }

    void FillFiller(const pos &ferz, const pos &step, vector<pos> &poses) {
        pos position = ferz;
        position.first += step.first;
        position.second += step.second;
        while(IsOnField(position) && field(position) != 2 && field(position) != 4) {
            poses.emplace_back(position);
            position.first += step.first;
            position.second += step.second;
        }
    }

    void PrepareField(const data &step_data) {
        field(step_data.king) = 4;
        for(int i = -1; i < 2; ++i) {
            for(int j = -1; j < 2; ++j) {
                if(i != 0 || j != 0) {
                    FieldFiller(step_data.ferz, {i, j});
                }
            }
        }
        field(step_data.ferz) = 3;
    }

    void FieldFiller(const pos &ferz, const pos &step) {
        pos position = ferz;
        while(IsOnField(position) && field(position) != 2) {
            field(position) = 1;
            position.first += step.first;
            position.second += step.second;
        }
    }

    bool IsOnField(const pos &cell) {
        return !(min(cell.first, cell.second) < 0 || max(cell.first, cell.second) > 7);
    }

    static bool RestoreField(size_t i, size_t j) {
        pos curr(i, j);
        field(curr) = 0;
        if(GameGraph::Manhattan(curr, graph.wk()) == 1) {
            field(curr) = 1;
        }
        else if(GameGraph::Manhattan(curr, graph.wk()) == 0) {
            field(curr) = 2;
        }
    }

    short &field(const pos &position) {
        return storage[position.first][position.second];
    }
private:
    vector<vector<short> > storage;
    GameGraph &graph;
};

int main() {

    return 0;
}