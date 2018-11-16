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

struct odometry {
    double old_x, new_x;
    double old_y, new_y;
    double old_angle, new_angle;
    size_t time;
    odometry(double a1, double a2, double b1, double b2, double c1, double c2, size_t mes_time);
};

class ParticleFilter {
public:
    explicit ParticleFilter(const JsonField &, state initial_robot_state);
    void PassNewOdometry(odometry measurement);
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
    void LowVarianceResample(const std::vector<double> &weights);
private:
    JsonField field;
    std::vector<state> particles;
    dot velocity;
    state robot;
    std::chrono::_V2::system_clock::time_point time = std::chrono::system_clock::now();
};


#endif //PARTICLEFILTER_PARTICLEFILTER_H
