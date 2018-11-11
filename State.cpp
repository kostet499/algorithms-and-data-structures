//
// Created by robocup on 11.11.18.
//

#include "State.h"

dot::dot(double value1, double value2) :
x(value1),
y(value2) {}

// supposed to be used when position of object is undefined
dot::dot() :
x(-1),
y(-1) {}

StaticState::StaticState(double width, double height, const std::vector<dot> &objects) :
field_width(width),
field_height(height),
static_field_objects(objects) {}

DynamicState::DynamicState(double head, double body, dot robot, dot ball, dot robot_v, dot ball_v) :
head_angle(head),
body_angle(body),
robot_position(robot), ball_position(ball),
robot_velocity(robot_v),
ball_velocity(ball_v) {
    time = std::chrono::system_clock::now();
}