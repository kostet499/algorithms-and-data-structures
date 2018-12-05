#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <queue>

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
    explicit GameGraph() {
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
                                SetUndefined(to_insert);
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
        // позиции фигур совпадают или короли рядом
        if(sit.king == sit.ferz || king == sit.ferz || MaxMetrics(king, sit.king) < 2) {
            return false;
        }
        // если во время хода ферзя король под боем
        if(!sit.king_turn && BeatenFerz(sit.king, sit)) {
            return false;
        }
        // позиции на поле
        return IsRightPos(sit.king) && IsRightPos(sit.ferz);
    }

    static int Manhattan(const pos &a, const pos &b) {
        return abs(a.first - b.first) + abs(a.second - b.second);
    }

    static int MaxMetrics(const pos &a, const pos &b) {
        return std::max(abs(a.first - b.first), abs(a.second - b.second));
    }

    bool IsRightPos(const pos &a) const {
        return !(a.first < 0 || a.second < 0 || a.first >= width_ || a.second >= height_);
    }

    bool BeatenFerz(const pos &check, const data &data_) const {
        const pos &ferz = data_.ferz;
        int dif_fst = abs(ferz.first - check.first);
        int dif_scd = abs(ferz.second - check.second);
        return (check != ferz) && (dif_fst == 0 || dif_scd == 0 || dif_fst == dif_scd) &&
                                  (dif_fst != (abs(ferz.first - king.first) + abs(king.first - check.first)) ||
                                   dif_scd != (abs(ferz.second - king.second) + abs(king.second - check.second)));
    }

    bool IsUndefined(const data &data_) {
        return storage[data_.hash()] == 100;
    }

    void SetUndefined(const data &data_) {
        storage[data_.hash()] = 100;
    }

    bool IsPat(const data &data_) {
        return storage[data_.hash()] == -1;
    }

    void SetPat(const data &data_) {
        storage[data_.hash()] = -1;
    }
private:
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
        while(!workflow.empty()) {
            data situation = workflow.front();
            workflow.pop();
            if (graph.IsUndefined(situation)) {
                if (situation.king_turn) {
                    tryKing(situation);
                } else {
                    tryFerz(situation);
                }
            }
        }
    }

    bool NoStep(const pos &check, const data &data_) const {
        for(int i = -1; i < 2; ++i) {
            for(int j = -1; j < 2; ++j) {
                if(j == 0 && i == 0) {
                    continue;
                }
                pos near_to_check(check.first + i, check.second + j);
                if(graph.IsRightPos(near_to_check) && !graph.BeatenFerz(near_to_check, data_)
                && GameGraph::MaxMetrics(near_to_check, graph.wk()) > 1) {
                    return false;
                }
            }
        }
        return true;
    }

    bool IsPat(const data &check) const {
        return check.king_turn && !graph.BeatenFerz(check.king, check) && NoStep(check.king, check);
    }

    bool IsMat(const data &check) const {
        return check.king_turn && graph.BeatenFerz(check.king, check) && NoStep(check.king, check);
    }
private:
    void initialize() {
        for (size_t i = 0; i < graph.size(); ++i) {
            if(IsPat(graph[i])) {
                // number for impossible cases
                graph.SetPat(graph[i]);
                feedFlow(graph[i]);
            }
            else if(IsMat(graph[i])) {
                graph[graph[i]] = 0;
                feedFlow(graph[i]);
            }
        }
    }

    void feedFlow(const data &sit) {
        if(sit.king_turn) {
            for(size_t i = 0; i < graph.width(); ++i) {
                for(size_t j = 0; j < graph.height(); ++j) {
                    data new_data = sit;
                    new_data.ferz = make_pair(i, j);
                    new_data.king_turn = false;
                    if(graph.IsRightData(new_data) && graph.BeatenFerz(new_data.ferz, sit)) {
                        workflow.push(new_data);
                    }
                }
            }
        }
        else {
            for(int i = -1; i < 2; ++i) {
                for(int j = -1; j < 2; ++j) {
                    if(i == 0 && j == 0) {
                        continue;
                    }
                    data new_data = sit;
                    new_data.king_turn = true;
                    new_data.king.first += i;
                    new_data.king.second += j;
                    if(graph.IsRightData(new_data)) {
                        workflow.push(new_data);
                    }
                }
            }
        }
    }

    // проверяет возможен ли был переход королём из позиции check в sit с максимизацией ходов
    // если встречает патовую вершину, то автоматически переходит в неё
    // ничего не делает, когда не определены все вершины
    void tryFerz(const data &sit) {
        graph[sit] = 0;
        for(size_t i = 0; i < graph.width(); ++i) {
            for(size_t j = 0; j < graph.height(); ++j) {
                data new_data = sit;
                new_data.ferz = make_pair(i, j);
                new_data.king_turn = true;
                if(graph.IsRightData(new_data) && graph.BeatenFerz(new_data.ferz, sit)) {
                    if(graph[sit] == 0 && graph.IsPat(new_data)) {
                        graph.SetPat(sit);
                    }
                    else if((graph[sit] == 0 || graph.IsPat(sit)) && graph.IsUndefined(new_data)) {
                        graph.SetUndefined(sit);
                    }
                    else if(!graph.IsPat(new_data) && !graph.IsUndefined(new_data)) {
                        graph[sit] = static_cast<short>(graph[new_data] + 1);
                    }
                }
            }
        }
        if(!graph.IsUndefined(sit)) {
            feedFlow(sit);
        }
    }

    void tryKing(const data &sit) {
        graph[sit] = 0;
        for(int i = -1; i < 2; ++i) {
            for(int j = -1; j < 2; ++j) {
                if(i == 0 && j == 0) {
                    continue;
                }

                data new_data = sit;
                new_data.king_turn = false;
                new_data.king.first += i;
                new_data.king.second += j;
                if(new_data.king == new_data.ferz && GameGraph::MaxMetrics(new_data.king, graph.wk()) > 1) {
                    graph[sit] = -1;
                    return;
                }

                if(!graph.IsRightData(new_data)) {
                    continue;
                }

                if(graph.IsPat(new_data)) {
                    graph.SetPat(sit);
                    return;
                }
                if(graph.IsUndefined(new_data)) {
                    graph.SetUndefined(sit);
                    return;
                }
                if(graph[new_data] + 1 > graph[sit]) {
                    graph[sit] = static_cast<short>(graph[new_data] + 1);
                }
            }
        }
        if(!graph.IsUndefined(sit)) {
            feedFlow(sit);
        }
    }
private:
    GameGraph &graph;
    queue<data> workflow;
};

// 19 матовых ситуаций вроде бы должно быть

int main() {
    GameGraph mygraph;
    Game mygame(mygraph);
    char s;
    int a, b, c, d;
    cin >> s;
    a = s - 'a';
    cin >> b;
    cin >> s;
    c = s - 'a';
    cin >> d;
    --b;
    --d;
    data mydata(make_pair(c, d), make_pair(a, b), false);
    if(mygraph[mydata] == -1) {
        cout << "IMPOSSIBLE";
    }
    else {
        cout << mygraph[mydata];
    }
    return 0;
}