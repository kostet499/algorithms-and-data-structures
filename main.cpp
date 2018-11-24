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

    data(pos f, pos k) : king(k), ferz(f) {
        step = 0;
    }

    // компаратор для set
    bool operator<(const data &other) const {
        return (score() < other.score()) ||
        (score() == other.score() && (compare_king(other) || (equal_kings(other) && (step < other.step || (step == other.step && compare_ferz(other))))));
    }

    bool compare_king(const data &other) const {
        return (king.second < other.king.second) || (king.second == other.king.second && king.first < other.king.first);
    }

    bool equal_kings(const data &other) const {
        return king == other.king;
    }

    bool compare_ferz(const data &other) const {
        return (ferz.second < other.ferz.second) || (ferz.second == other.ferz.second && ferz.first < other.ferz.first);
    }


    size_t score() const {
        return step +
        abs(king.first - 2) + abs(king.second - 2) +
        abs(king.first - ferz.first) + abs(king.second - king.second) +
        abs(king.second) + abs(king.first);
    }
};

class Game {
public:
    explicit Game(data &start) {
        storage.resize(8);
        for(auto &filed : storage) {
            filed.resize(8, 0);
        }

        field({2, 2}) = 2;
        if(start.king.first > start.king.second) {
            swap(start.king.first, start.king.second);
            swap(start.ferz.first, start.ferz.second);
        }
        RestoreField();
        dataset.insert(start);
    }

    // собственно сам алгоритм A*, который вернет найденный ответ
    // если он не пришёл к ответу, то вернёт -1
    int AStar(size_t iterations_limit) {
        global_answer = -1;
        bool is_first = true;
        size_t iterations = 0;
        while(!dataset.empty() && iterations < iterations_limit) {
            ++iterations;
            data new_step_data = *dataset.begin();
            dataset.erase(dataset.begin());
            PrepareField(new_step_data);
            int step_answer;
            step_answer = MakeStep(new_step_data, is_first);
            RestoreField();
            if(step_answer != -1 && (step_answer < global_answer || global_answer == -1)) {
                global_answer = step_answer;
            }
            is_first = false;
        }
        return global_answer;
    }
private:
    // тут первым ходит король, следовательно в самом начале делаю первым специально ход за ферзя
    int MakeStep(data &pst, bool is_first) {
        if(is_first) {
            FillDataset(pst);
            return -1;
        }
        int answer = -1;
        // если 0, то ничего не определено, 1 - пришел к победе или пат - мы проиграли, 2 - мат- победа
        int result = KingStep(pst);
        if(result == 2) {
            answer = static_cast<int>(pst.step);
        }
        else if(result == 0) {
            // добавление новых позиций в датасэт
            field(pst.king) = 4;
            FillDataset(pst);
        }
        return answer;
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

    // король либо в патовой ситуации, либо в матовой, либо он пришёл к победе
    int KingStep(data &pst) {
        // выбираем по Score наибольшую, надеясь на то, что она оптимально-стратегическая
        vector<data> ways;
        data new_data = pst;
        ++new_data.step;
        for(int i = -1; i < 2; ++i) {
            for(int j = -1; j < 2; ++j) {
                if(j != 0 || i != 0) {
                    new_data.king = make_pair(pst.king.first + i, pst.king.second + j);
                    if(IsOnField(new_data.king) && field(new_data.king) != 1) {
                        if(field(new_data.king) == 3 || new_data.king.second > 5) {
                            // фактически мы же не ограничивали положение ферзя, так что он мог стать под бой - это поражение
                            // также мы посчитали, что короля нельзя изгнать из самых высоких двух рядов
                            return 1;
                        }
                        else {
                            ways.emplace_back(new_data);
                        }
                    }
                }
            }
        }
        // если король не может никуда пойти и поле помечено шахом, то это мат
        if(ways.empty() && field(pst.king) == 1) {
            return 2;
        }
        else if(ways.empty()) {
            return 1;
        }
        else {
            sort(ways.begin(), ways.end());
            pst.king = ways.back().king;
            ++pst.step;
            return 0;
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

    void RestoreField() {
        for(auto &filed : storage) {
            fill(filed.begin(), filed.end(), 0);
        }
        for(size_t i = 1; i < 4; ++i) {
            for(size_t j = 1; j < 4; ++j) {
                field({i, j}) = 1;
            }
        }
        field({2, 2}) = 2;
    }

    short &field(const pos &position) {
        return storage[position.first][position.second];
    }
private:
    vector<vector <short> > storage;
    set<data> dataset;
    int global_answer;
};

int main() {
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
    data mydata({a, b}, {c, d});
    Game mygame(mydata);
    int answer = mygame.AStar(1000000);
    if(answer == -1) {
        cout << "IMPOSSIBLE";
    }
    else {
        cout << answer;
    }
    return 0;
}