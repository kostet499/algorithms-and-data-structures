#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <queue>
#include <set>
#include <cmath>

using namespace std;

using pos = pair<short, short>;

struct state {
    pos king;
    pos ferz;
    bool king_turn;

    state(pos a, pos b, bool d) : king(std::move(a)), ferz(std::move(b)), king_turn(d) {
    }

    bool operator==(const state &other) const{
        return other.king == king && other.ferz == ferz && other.king_turn == king_turn;
    }
};

namespace std {
    template <>
    struct hash<state> {
        using val_t = unsigned long long;
        val_t operator()(const state &a) const{
            return static_cast<val_t>(a.king_turn) +
                   (static_cast<val_t>(a.king.first) << 1) +
                   (static_cast<val_t>(a.king.second) << 9) +
                   (static_cast<val_t>(a.ferz.first) << 17) +
                   (static_cast<val_t>(a.ferz.second) << 25);
        }

    };
}

class GameGraph {
public:
    explicit GameGraph(size_t width__, size_t height__, pos king_) :
    width_(width__), height_(height__), wk(std::move(king_)) {
        for(size_t i = 0; i < width_; ++i) {
            for(size_t j = 0; j < height_; ++j) {
                for(size_t k = 0; k < width_; ++k) {
                    for(size_t h = 0; h < height_; ++h) {
                        for(size_t is_true = 0; is_true < 2; ++is_true) {
                            state to_insert = state(make_pair(i, j), make_pair(k, h),static_cast<bool>(is_true));
                            if(IsRightState(to_insert)) {
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

    short& operator[](const state &i) {
        return storage[i];
    }

    const state& operator[](int i) {
        return iterated[i];
    }

    const pos& king() {
        return wk;
    }

    const size_t size() {
        return iterated.size();
    }

    static double euclidean(const pos &a, const pos &b) {
        return sqrt(pow(a.first - b.first, 2) + pow(a.second - b.second, 2));
    }

    static bool IsBetween(const pos &a, const pos &b, const pos &c) {
        double ab = euclidean(a, b);
        double ac = euclidean(a, c);
        double bc = euclidean(b, c);
        return ac + bc - ab < 1e-8;
    };

    bool IsRightState(const state &sit) const {
        // позиции фигур совпадают или короли рядом
        if(sit.king == sit.ferz || wk == sit.ferz || MaxMetrics(wk, sit.king) < 2) {
            return false;
        }
        // если во время хода ферзя король под боем
        if(!sit.king_turn && (BeatenFerz(sit.king, sit) || Manhattan(sit.king, wk) < 2)) {
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

    bool BeatenFerz(const pos &check, const state &state_) const {
        const pos &ferz = state_.ferz;
        int dif_fst = abs(ferz.first - check.first);
        int dif_scd = abs(ferz.second - check.second);
        return (check != ferz) && (dif_fst == 0 || dif_scd == 0 || dif_fst == dif_scd) && !IsBetween(check, ferz, wk);
    }

    bool IsUndefined(const state &state_) {
        return storage[state_] == 100;
    }

    void SetUndefined(const state &state_) {
        storage[state_] = 100;
    }

    bool IsPat(const state &state_) {
        return storage[state_] == -1;
    }

    void SetPat(const state &state_) {
        storage[state_] = -1;
    }
private:
    unordered_map<state, short> storage;
    std::vector<state> iterated;
    pos wk;
    size_t width_;
    size_t height_;
};

class Game {
public:
    explicit Game(GameGraph &gamegraph) : graph(gamegraph) {
        initialize();
        while(!workflow.empty()) {
            state situation = workflow.front();
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
private:
    void initialize() {
        for(size_t i = 0; i < graph.size(); ++i) {
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

    bool NoStep(const pos &check, const state &state_) const {
        for(int i = -1; i < 2; ++i) {
            for(int j = -1; j < 2; ++j) {
                if(j == 0 && i == 0) {
                    continue;
                }
                pos near_to_check(check.first + i, check.second + j);
                if(graph.IsRightPos(near_to_check) && !graph.BeatenFerz(near_to_check, state_)
                   && GameGraph::MaxMetrics(near_to_check, graph.king()) > 1) {
                    return false;
                }
            }
        }
        return true;
    }

    bool IsPat(const state &check) const {
        return check.king_turn && !graph.BeatenFerz(check.king, check) && NoStep(check.king, check);
    }

    bool IsMat(const state &check) const {
        return check.king_turn && graph.BeatenFerz(check.king, check) && NoStep(check.king, check);
    }

    void feedFlow(const state &sit) {
        if(sit.king_turn) {
            for(size_t i = 0; i < graph.width(); ++i) {
                for(size_t j = 0; j < graph.height(); ++j) {
                    state new_state = sit;
                    new_state.ferz = make_pair(i, j);
                    new_state.king_turn = false;
                    if(graph.IsRightState(new_state) && graph.BeatenFerz(new_state.ferz, sit)) {
                        workflow.push(new_state);
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
                    state new_state = sit;
                    new_state.king_turn = true;
                    new_state.king.first += i;
                    new_state.king.second += j;
                    if(graph.IsRightState(new_state)) {
                        workflow.push(new_state);
                    }
                }
            }
        }
    }

    // проверяет возможен ли был переход королём из позиции check в sit с максимизацией ходов
    // если встречает патовую вершину, то автоматически переходит в неё
    // ничего не делает, когда не определены все вершины
    void tryFerz(const state &sit) {
        if(graph.IsUndefined(sit)) {
            graph[sit] = 0;
        }
        for(size_t i = 0; i < graph.width(); ++i) {
            for(size_t j = 0; j < graph.height(); ++j) {
                state new_state = sit;
                new_state.ferz = make_pair(i, j);
                new_state.king_turn = true;
                if(graph.IsRightState(new_state) && graph.BeatenFerz(new_state.ferz, sit)) {
                    if(graph[sit] == 0 && graph.IsPat(new_state)) {
                        graph.SetPat(sit);
                    }
                    else if((graph[sit] == 0 || graph.IsPat(sit)) && graph.IsUndefined(new_state)) {
                        graph.SetUndefined(sit);
                    }
                    else if(!graph.IsPat(new_state) && !graph.IsUndefined(new_state)) {
                        if(graph[sit] == 0) {
                            graph[sit] = static_cast<short>(graph[new_state] + 1);
                        }
                        else {
                            graph[sit] = std::min(graph[sit], static_cast<short>(graph[new_state] + 1));
                        }
                    }
                }
            }
        }
        if(!graph.IsUndefined(sit)) {
            feedFlow(sit);
        }
    }

    void tryKing(const state &sit) {
        graph[sit] = 0;
        for(int i = -1; i < 2; ++i) {
            for(int j = -1; j < 2; ++j) {
                if(i == 0 && j == 0) {
                    continue;
                }

                state new_state = sit;
                new_state.king_turn = false;
                new_state.king.first += i;
                new_state.king.second += j;
                if(new_state.king == new_state.ferz && GameGraph::MaxMetrics(new_state.king, graph.king()) > 1) {
                    graph.SetPat(sit);
                    feedFlow(sit);
                    return;
                }

                if(!graph.IsRightState(new_state)) {
                    continue;
                }

                if(graph.IsPat(new_state)) {
                    graph.SetPat(sit);
                    feedFlow(sit);
                    return;
                }
                if(graph.IsUndefined(new_state)) {
                    graph.SetUndefined(sit);
                    return;
                }
                if(graph[new_state] + 1 > graph[sit]) {
                    graph[sit] = static_cast<short>(graph[new_state] + 1);
                }
            }
        }
        feedFlow(sit);
    }
private:
    GameGraph &graph;
    queue<state> workflow;
};

// 19 матовых ситуаций вроде бы должно быть

int main() {
    GameGraph graph(8, 8, pos(2, 2));
    Game game(graph);
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
    state state_(make_pair(c, d), make_pair(a, b), false);
    if(graph.IsPat(state_)) {
        cout << "IMPOSSIBLE";
    }
    else {
        cout << graph[state_];
    }
    return 0;
}