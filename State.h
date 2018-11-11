//
// Created by robocup on 11.11.18.
//

#ifndef PARTICLEFILTER_STATE_H
#define PARTICLEFILTER_STATE_H

#include <vector>
#include <chrono>

struct dot {
    double x;
    double y;

    dot(double, double);
    dot();
};

struct StaticState {
    double field_width;
    double field_height;
    std::vector<dot> static_field_objects;

    StaticState(double, double, const std::vector<dot> &);
};

struct DynamicState {
    double head_angle;
    double body_angle;
    dot robot_position;
    dot ball_position;
    dot robot_velocity;
    dot ball_velocity;
    std::chrono::_V2::system_clock::time_point time;

    DynamicState(double, double, dot, dot, dot, dot);
};


#endif //PARTICLEFILTER_STATE_H
