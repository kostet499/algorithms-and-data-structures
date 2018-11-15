#include "ParticleFilter.h"
#include <iostream>

int main() {
    JsonField field("field.json");
    std::cout << field[2].first.x << " " << field[2].first.y << std::endl;
    std::cout << field.width() << " " << field.height() << std::endl;
    boost::normal_distribution<double> cdf(3, 0.6);
    boost::taus88 gen(static_cast<const unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    std:: cout << cdf(gen);
    return 0;
}