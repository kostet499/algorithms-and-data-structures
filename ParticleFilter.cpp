//
// Created by robocup on 11.11.18.
//

#include "ParticleFilter.h"
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
state::state(dot pos, double k) : position(pos), angle(k) {}
state::state() : state(dot(0.0, 0.0), 0.0){}

ParticleFilter::ParticleFilter(const JsonField &f, state initial_robot_state, size_t amount)
: field(f), robot(initial_robot_state), particles_amount(amount){
    weights.resize(particles_amount, 1.0 / particles_amount);
    particles.resize(particles_amount);
    // раскидываем частички по полю
    const unsigned int generator_seed = static_cast<const unsigned  int> (std::chrono::system_clock::now().time_since_epoch().count());
    boost::taus88 generator(generator_seed);

    boost::random::uniform_real_distribution<double> x_coord(0.0, field.width());
    boost::random::uniform_real_distribution<double> y_coord(0.0, field.height());
    boost::random::uniform_real_distribution<double> angle(0.0, 2 * M_PI);
    for(auto &particle : particles) {
        particle.position.x = x_coord(generator);
        particle.position.y = y_coord(generator);
        particle.angle = angle(generator);
    }
}

void ParticleFilter::PassNewVision(const char *filename) {
    JsonObjectsSeen objects(filename);
    // мы должны сопоставить линии, которые мы видим линиям поля
    std::vector<double> mistakes(particles_amount);
    for(size_t i = 0; i < particles.size(); ++i) {
        // сначала нужно перейти от систем координат робота к новым координатам
        auto objects_ns = objects.GetLinesSeen();
        for(auto &object : objects_ns) {
            SetToNewSystem(particles[i], object.first);
            SetToNewSystem(particles[i], object.second);
        }
        // в данный момент это ошибки
        mistakes[i] = ChooseBestFit(particles[i], objects_ns, ScoreLine);
    }

    MistakesToProbability(mistakes);
    weights = mistakes;

    // получаем новую позицию - просто берем среднее по координатам
    robot.position = dot(0.0, 0.0);
    for(size_t i = 0; i < particles.size(); ++i) {
        robot.position = robot.position + particles[i].position * weights[i];
        robot.angle += particles[i].angle * weights[i];
    }

    // оставляем только 30 частиц например из 100.
    LowVarianceResample(30);
    // дополняем до нужного числа частиц
}

void ParticleFilter::LowVarianceResample(size_t particles_count) {
    std::vector<state> new_particles;
    std::vector<state> new_weigths;
    const unsigned int generator_seed = static_cast<const unsigned  int> (std::chrono::system_clock::now().time_since_epoch().count());
    boost::taus88 generator(generator_seed);

    double weight = weights[0];
    size_t random_number = generator() % (particles_count + 1);
    double number = random_number == 0 ? 0 : 1.0 / random_number;
    size_t i = 0;

    double weight_sum = 0.0;
    for(size_t m = 0; m < particles_count; ++m) {
        double uber = number + static_cast<double>(m) / particles_count;
        while(uber > weight) {
            ++i;
            weight += weights[i];
        }
        new_particles.emplace_back(particles[i]);
        new_weigths.emplace_back(weights[i]);
        weight_sum += weights[i];
    }
    // нормализуем новые веса
    for(auto &wes : weights) {
        wes /= weight_sum;
    }
    // замещаем частицы новой моделью
    particles = new_particles;
}

void ParticleFilter::MistakesToProbability(std::vector<double> &mistakes) {
    // заглушка
    // нужно вообще говоря подумать, что тут лучше сделать
}

double ParticleFilter::ChooseBestFit(const state &particle, const std::vector<line> &lines_seen,
                                         double (*GiveScore)(const state &, const line &, const line &) ) {
    // заглушка
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

odometry::odometry(double a1, double a2, double b1, double b2, double c1, double c2, size_t mes_time) {
    old_x = a1;
    new_x = a2;
    old_y = b1;
    new_y = b2;
    old_angle = c1;
    old_angle = c2;
    time = mes_time;
}

// нужно разобраться с одометрией, смущают больше всего углы - от чего их отсчитывать?
void ParticleFilter::PassNewOdometry(odometry od) {
    auto current_time = std::chrono::system_clock::now();
    std::vector<state> new_particles(particles.size());
    // глобальная ось OY, пока что с ней считаю угол
    dot OY(0, 1);

    double rot1 = ComputeAngle(OY, dot(od.new_x - od.old_x, od.new_y - od.old_y)) - od.old_angle;
    double shift = dot(od.new_x - od.old_x, od.new_y - od.old_y).norm();
    double rot2 = od.new_angle - od.old_angle - rot1;

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