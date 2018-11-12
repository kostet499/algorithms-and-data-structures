//
// Created by robocup on 12.11.18.
//

#include "JsonField.h"
#include <jsoncpp/json/json.h>
#include <iostream>
#include <string>

dot::dot(double value1, double value2) :
        x(value1),
        y(value2) {}

// supposed to be used when position of object is undefined
dot::dot() :
        x(-1),
        y(-1) {}

JsonField::JsonField(const char *filename) {
    std::freopen(filename, "r", stdin);

    Json::Value root;
    std::cin >> root;

    std::string endcoding = root.get("encoding", "UTF-8").asString();

    const Json::Value field = root["field"];
    for(auto &val : field) {
        dot first(val["x1"].asDouble(), val["y1"].asDouble());
        dot second(val["x2"].asDouble(), val["y2"].asDouble());
        static_field_objects.emplace_back(first);
        static_field_objects.emplace_back(second);
        field_height = std::max(first.y, std::max(second.y, field_height));
        field_width = std::max(first.x, std::max(second.x, field_width));
    }
}

const dot& JsonField::operator[](size_t index) {
    return static_field_objects[index];
}

double JsonField::width() const {
    return field_width;
}

double JsonField::height() const {
    return field_height;
}

double dot::norm() {
    return std::sqrt(pow(x, 2) + pow(y, 2));
}