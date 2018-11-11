//
// Created by robocup on 11.11.18.
//

#ifndef PARTICLEFILTER_PARTICLEFILTER_H
#define PARTICLEFILTER_PARTICLEFILTER_H

#include "State.h"


class ParticleFilter {
public:
    ParticleFilter(const StaticState &);

private:
    std::vector<DynamicState> motion_model();

private:
    StaticState field;
    std::vector<DynamicState> particles;
};


#endif //PARTICLEFILTER_PARTICLEFILTER_H
