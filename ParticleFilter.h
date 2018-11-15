//
// Created by robocup on 11.11.18.
//

#ifndef PARTICLEFILTER_PARTICLEFILTER_H
#define PARTICLEFILTER_PARTICLEFILTER_H

#include "JsonObjectsSeen.h"
#include <vector>
#include <chrono>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/taus88.hpp>
#include <cmath>

struct state {
    double x, y;
    // угол оси OY робота с глобальной осью OY, робот "смотрит" без поворота головы вдоль своей OY
    double angle;

    state(double, double, double);
    state();
};


class ParticleFilter {
public:
    explicit ParticleFilter(const JsonField &);
    void PassNewVelocity(dot velocity);
    void PassNewVision(const char *filename);
private:
    dot Motion(std::chrono::_V2::system_clock::time_point current_time, const dot &new_velocity) const;
    double score_line(const line &a, const line &b) const;
    double line_angle(const line &a, const line &b) const;
    double angle(const dot &a, const dot &b) const;
    double angle_penalty(double angle) const;
    double distance_penalty(double distance) const;
    double line_distance(const line &a, const line &b) const;
private:
    JsonField field;
    std::vector<state> particles;
    dot velocity;
    std::chrono::_V2::system_clock::time_point time = std::chrono::system_clock::now();
};


#endif //PARTICLEFILTER_PARTICLEFILTER_H
