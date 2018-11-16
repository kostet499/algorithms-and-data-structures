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
    void PassNewOdometry(std::vector<dot> odometry);
    void PassNewVision(const char *filename);
private:
    dot ComputeShift(std::chrono::_V2::system_clock::time_point current_time, const dot &new_velocity) const;
    static double ScoreLine(const state &particle, const line &a, const line &b);
    static double LineAngle(const line &a, const line &b);
    static double ComputeAngle(const dot &a, const dot &b);
    static double AnglePenalty(double angle);
    static double DistancePenalty(double distance);
    static double DistanceRobotToLine(const state &particle, const line &b);
    static double ChooseBestFit(const state &particle, const std::vector<line> &lines_seen,
            double (*GiveScore)(const state &, const line &, const line &) );
    static void MistakesToProbability(std::vector<double> &mistakes);
    static void SetToNewSystem(const state &particle, dot &object);
private:
    JsonField field;
    std::vector<state> particles;
    dot velocity;
    dot robot_position;
    std::chrono::_V2::system_clock::time_point time = std::chrono::system_clock::now();
};


#endif //PARTICLEFILTER_PARTICLEFILTER_H
