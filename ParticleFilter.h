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
    dot position;
    // угол оси OY робота с глобальной осью OY, робот "смотрит" без поворота головы вдоль своей OY
    double angle;

    state(dot, double);
    state();
};


class ParticleFilter {
public:
    explicit ParticleFilter(const JsonField &);
    void PassNewVelocity(dot velocity);
    void PassNewVision(const char *filename);
private:
    dot ComputeShift(std::chrono::_V2::system_clock::time_point current_time, const dot &new_velocity) const;
    double ScoreLine(const line &a, const line &b) const;
    double LineAngle(const line &a, const line &b) const;
    double ComputeAngle(const dot &a, const dot &b) const;
    double AnglePenalty(double angle) const;
    double DistancePenalty(double distance) const;
    double LineDistance(const line &a, const line &b) const;
    void SetToNewSystem(const state &particle, dot &object) const;
private:
    JsonField field;
    std::vector<state> particles;
    dot velocity;
    std::chrono::_V2::system_clock::time_point time = std::chrono::system_clock::now();
};


#endif //PARTICLEFILTER_PARTICLEFILTER_H
