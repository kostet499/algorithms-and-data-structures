//
// Created by robocup on 11.11.18.
//

#include "State.h"



StaticState::StaticState(double width, double height, const std::vector<dot> &objects) :
    field_width(width),
    field_height(height),
    static_field_objects(objects) {}

DynamicState::DynamicState(double body, dot pos) :
    body_angle(body),
    robot(pos){}