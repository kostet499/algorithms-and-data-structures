//
// Created by robocup on 11.11.18.
//

/*
 * Мы представляем частицу как координаты центра масс робота x,y
 * и угол поворота относительно оси, выбранной заранее.
 *
 * Также мы оперируем вектором скорости, который измеряется в системе отсчёта робота.
 * Декартовая система координат робота - ОХ направлена в сторону правого плеча, ОУ перпендикулярна ОХ
 * и идёт по направлению взгляда робота.  Начало отсчёта - проекция центра масс на плоскость
 *
 * Так же, неординарный выбор. Каждый раз в функции PassNewVelocity мы используем для перемещения не новую
 * скорость, а старую (которая была в предыдущий раз передана в эту функцию). То есть в предыдущий момент мы запомнили
 * скорость и момент времени передачи этой скорости. Когда мы еще раз функцию, то мы используем старые данные, чтобы
 * посчитать вектор перемещения и переместиться, и запоминаем новые данные.
 *
 * В качестве распределения используем нормальное. По идее на роботе используется boost так, что я спокойно могу
 * пользоваться разными функциями из их библиотек. Взял реализацию нормального распределения первым методом Бокса-Мюллера
 * через тригонометрические функции (точно и вроде бы быстро). В качестве генератора равномерно распределённых величин
 * для этого метода использую вроде как самый быстрый taus88 из буста.
 */

#ifndef PARTICLEFILTER_PARTICLEFILTER_H
#define PARTICLEFILTER_PARTICLEFILTER_H

#include "JsonField.h"
#include <vector>
#include <chrono>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/taus88.hpp>

struct state {
    double x, y;
    double angle;

    state(double, double, double);
    state();
};


class ParticleFilter {
public:
    explicit ParticleFilter(const JsonField &);
    void PassNewVelocity(dot velocity);
    void PassNewVision(const std::vector<dot> &objects);
private:
    dot Motion(std::chrono::_V2::system_clock::time_point current_time, const dot &new_velocity) const;
private:
    JsonField field;
    std::vector<state> particles;
    dot velocity;
    std::chrono::_V2::system_clock::time_point time = std::chrono::system_clock::now();
};


#endif //PARTICLEFILTER_PARTICLEFILTER_H
