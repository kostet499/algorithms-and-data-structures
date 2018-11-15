//
// Created by robocup on 11.11.18.
//

#include "ParticleFilter.h"

state::state(dot pos, double k) : position(pos), angle(k) {}
state::state() : state(dot(0.0, 0.0), 0.0){}

ParticleFilter::ParticleFilter(const JsonField &f) : field(f) {}

void ParticleFilter::PassNewVision(const char *filename) {
    JsonObjectsSeen objects(filename);
    std::vector<double> weights(particles.size());
    // мы должны сопоставить линии, которые мы видим линиям поля

    for(size_t i = 0; i < particles.size(); ++i) {
        // сначала нужно перейти от систем координат робота к новым координатам
        auto objects_ns = objects.GetLinesSeen();
        for(auto &object : objects_ns) {
            SetToNewSystem(particles[i], object.first);
            SetToNewSystem(particles[i], object.second);
        }
        // в данный момент это ошибки
        weights[i] = ChooseBestFit(particles[i], objects_ns, ScoreLine);
    }

    MistakesToProbability(weights);


}

void ParticleFilter::MistakesToProbability(std::vector<double> &mistakes) {
    // заглушка
    // нужно вообще говоря подумать, что тут лучше сделать
}

double ParticleFilter::ChooseBestFit(const state &particle, const std::vector<line> &lines_seen,
                                         double (*GiveScore)(const state &, const line &, const line &) ) {
    // теперь мы должны сопоставить линии, которые мы видим, линиям поля
    // я предлагаю для каждой видимой линии ставить в соответствие ей линию поля
    // * тут хочет быть венгерский алгоритм *
    return 2281337.0;
}

// координаты точки приводятся к системе координат поля
void ParticleFilter::SetToNewSystem(const state &particle, dot &object) {
    // поворот - тут матрицу нужно проверить
    dot rotated(
            object.x * cos(particle.angle) + object.y * sin(particle.angle),
            object.x * (-sin(particle.angle)) + object.y * cos(particle.angle)
            );
    // параллельный перенос
    object = rotated + particle.position;
}

// угол между векторами
double ParticleFilter::ComputeAngle(const dot &a, const dot &b){
    // http://qaru.site/questions/143580/direct-way-of-computing-clockwise-angle-between-2-vectors
    double one = a.x * b.x + a.y * b.y;
    double two = a.x * b.y - a.y * b.x;
    return atan2(two, one);
}

// угол между линиями 0 до 90 градусов
double ParticleFilter::LineAngle(const line &a, const line &b) {
    double ax = a.first.x - a.second.x;
    double ay = a.first.y - a.second.y;
    double bx = b.first.x - b.second.x;
    double by = b.first.y - b.second.y;
    double one = fabs(ax * bx + ay * by); // фактически как модуль от косинуса
    double two = fabs(ax * by - ay * bx); // аналогично модуль от синуса
    return atan2(two, one);
}

// расстояние между роботом и линией в глобальной системе координат
// использовал своё аналитическое решение, которые уже было протестировано в другой задаче
double ParticleFilter::DistanceRobotToLine(const state &particle, const line &liny) {
    auto direction = liny.second - liny.first;
    auto touch = particle.position - liny.first;
    auto angle_cos = ComputeAngle(touch, direction);
    auto parameter = touch(direction) / pow(direction.norm(), 2);

    if(direction.norm() < 1e-8 || 0.0 > parameter) {
        return (liny.first - particle.position).norm();
    }
    if(parameter > 1.0) {
        return (liny.second - particle.position).norm();
    }

    double sq_cos = pow(angle_cos, 2);
    double sinus = sq_cos > 1 ? 0 : sqrt(1 - sq_cos);

    return touch.norm() * sinus;
}

// функции штрафов, можем делать что угодно)
double ParticleFilter::AnglePenalty(double angle) {
    return angle * 1e1;
}

double ParticleFilter::DistancePenalty(double distance) {
    return distance * 1e1;
}

// функция для определения "различия" линий
// штрафуем за разницу направлений линий
// штрафуем за разницу расстояний от робота до линий
double ParticleFilter::ScoreLine(const state &particle, const line &a, const line &b) {
    return AnglePenalty(LineAngle(a, b)) +
    DistancePenalty(fabs(DistanceRobotToLine(particle, a) - DistanceRobotToLine(particle, b)));
}

// Я, кажется, неправильно построил модель скорости
void ParticleFilter::PassNewVelocity(dot new_velocity) {
    auto current_time = std::chrono::system_clock::now();
    std::vector<state> new_particles(particles.size());

    // параметры, которые мы вольны менять
    double sigma_shift = 0.5;
    double sigma_angle = 0.5;
    const unsigned int generator_seed = static_cast<const unsigned  int> (current_time.time_since_epoch().count());

    // радиус вектор перемещения в системе отсчёта робота
    dot shift = ComputeShift(current_time, new_velocity);

    // ось ОУ робота
    dot OY(0, 1);

    boost::taus88 generator(generator_seed);
    // выдаёт модуль скорости + гауссовский шум
    boost::normal_distribution<double> absolute_shift_noise(shift.norm(), sigma_shift);
    // выдаёт угол с осью ОУ + гауссовский шум
    boost::normal_distribution<double> vector_angle_noise(ComputeAngle(OY, shift), sigma_angle);

    for(auto &particle : particles) {
        double angle = vector_angle_noise(generator);
        double absolute_shift = absolute_shift_noise(generator);
        particle.position.x += absolute_shift * cos(angle);
        particle.position.y += absolute_shift * sin(angle);
    }

    velocity = new_velocity;
    time = current_time;
}

dot ParticleFilter::ComputeShift(std::chrono::_V2::system_clock::time_point current_time, const dot &new_velocity) const {
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - time);
    double time_in_secs = millisecs.count() * 1e-3;
    return {velocity.x * time_in_secs, velocity.y * time_in_secs};
}

