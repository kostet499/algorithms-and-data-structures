//
// Created by robocup on 11.11.18.
//

#include "ParticleFilter.h"

state::state(double i, double j, double k) : x(i), y(j), angle(k) {}
state::state() : state(0.0, 0.0, 0.0){}

ParticleFilter::ParticleFilter(const JsonField &f) : field(f) {}

void ParticleFilter::PassNewVelocity(dot new_velocity) {
    auto current_time = std::chrono::system_clock::now();
    std::vector<state> new_particles(particles.size());

    // параметры, которые мы вольны менять
    double sigma_shift = 0.5;
    double sigma_angle = 0.5;
    const unsigned int generator_seed = static_cast<const unsigned  int> (current_time.time_since_epoch().count());


    // радиус вектор перемещения в системе отсчёта робота
    dot shift = Motion(current_time, new_velocity);

    boost::taus88 generator(generator_seed);
    boost::normal_distribution<double> absolute_shift_noise(shift.norm(), sigma_shift);
    // подумаю еще как представлять направление вектора ...
    boost::normal_distribution<double> vector_angle_noise(, sigma_angle);


    for(const auto &particle : particles) {
        // code ...
    }

    velocity = new_velocity;
    time = current_time;
}


// зависит от единиц измерения скорости
// в данный момент предполагаю, что всё в системе СИ
// можно доработать основываясь на изменении самого вектора скорости от старого к новому
// пока что я просто умножаю прошлый вектор скорости на прошедшее время
dot ParticleFilter::Motion(std::chrono::_V2::system_clock::time_point current_time, const dot &new_velocity) const {
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - time);
    double time_in_secs = millisecs.count() / 1000.0;
    return {velocity.x * time_in_secs, velocity.y * time_in_secs};
}

