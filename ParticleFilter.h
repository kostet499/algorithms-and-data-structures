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
    void PassNewVision(const char *filename);
private:
    dot Motion(std::chrono::_V2::system_clock::time_point current_time, const dot &new_velocity) const;
private:
    JsonField field;
    std::vector<state> particles;
    dot velocity;
    std::chrono::_V2::system_clock::time_point time = std::chrono::system_clock::now();
};


#endif //PARTICLEFILTER_PARTICLEFILTER_H
