//
// Created by robocup on 11.11.18.
//

#ifndef PARTICLEFILTER_PARTICLEFILTER_H
#define PARTICLEFILTER_PARTICLEFILTER_H

#include "JsonField.h"


class ParticleFilter {
public:
    explicit ParticleFilter(const JsonField &);

private:
    //std::vector<DynamicState> motion_model();

private:
    JsonField field;
};


#endif //PARTICLEFILTER_PARTICLEFILTER_H
