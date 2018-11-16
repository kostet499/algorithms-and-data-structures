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

// нужно проверять
void ParticleFilter::PassNewOdometry(std::vector<dot> odometry) {
    auto current_time = std::chrono::system_clock::now();
    std::vector<state> new_particles(particles.size());
    // глобальная ось OY
    dot OY(0, 1);

    double rot1 = ComputeAngle(OY, dot(odometry[0].y - odometry[0].x, odometry[1].y - odometry[1].x)) - odometry[2].x;
    double shift = dot(odometry[0].x - odometry[0].y, odometry[1].x - odometry[1].y).norm();
    double rot2 = odometry[2].y - odometry[2].y - rot1;

    // параметры шума
    double a1 = 0.5, a2 = 0.5, a3 = 0.5, a4 = 0.5;
    double sigma_rot1 = std::sqrt(a1 * pow(rot1, 2) + a2 * pow(shift, 2));
    double sigma_shift = std::sqrt(a3 * pow(shift, 2) + a4 * pow(rot1, 2) + a4 * pow(rot2, 2));
    double sigma_rot2 = std::sqrt(a1 * pow(rot2, 2) + a2 * pow(shift, 2));
    const unsigned int generator_seed = static_cast<const unsigned  int> (current_time.time_since_epoch().count());


    boost::taus88 generator(generator_seed);
    // гауссовский шум
    boost::normal_distribution<double> shift_noise(0.0, sigma_shift);
    boost::normal_distribution<double> rot1_noise(0.0, sigma_rot1);
    boost::normal_distribution<double> rot2_noise(0.0, sigma_rot2);

    for(auto &particle : particles) {
        double rot1_n = rot1 - rot1_noise(generator);
        double rot2_n = rot2 - rot2_noise(generator);
        double shift_n = shift - shift_noise(generator);

        particle.position.x = particle.position.x + shift_n * cos(particle.angle + rot1_n);
        particle.position.y = particle.position.y + shift_n * sin(particle.angle + rot1_n);
        particle.angle += rot1_n + rot2_n;
    }
}

// пока что не используется, так как предполагается, что одометрия, приходит уже готовой
dot ParticleFilter::ComputeShift(std::chrono::_V2::system_clock::time_point current_time, const dot &new_velocity) const {
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - time);
    double time_in_secs = millisecs.count() * 1e-3;
    return {velocity.x * time_in_secs, velocity.y * time_in_secs};
}

