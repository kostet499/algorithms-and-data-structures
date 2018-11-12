//
// Created by robocup on 11.11.18.
//

#ifndef PARTICLEFILTER_STATE_H
#define PARTICLEFILTER_STATE_H

#include <vector>
#include <chrono>



struct StaticState {
    double field_width;
    double field_height;
    std::vector<dot> static_field_objects;

    StaticState(double, double, const std::vector<dot> &);
};

struct DynamicState {
    double body_angle;
    dot robot;

    DynamicState(double, dot);
};


#endif //PARTICLEFILTER_STATE_H
