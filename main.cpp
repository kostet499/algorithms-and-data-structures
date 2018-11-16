#include "ParticleFilter.h"

int main() {
    JsonField field("field.json");
    std::cout << field[2].first.x << " " << field[2].first.y << std::endl;
    std::cout << "Field parameters " << field.width() << " " << field.height() << std::endl;
    boost::normal_distribution<double> cdf(3, 0.6);
    boost::taus88 gen(static_cast<const unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    std:: cout << "Generator test " <<  cdf(gen) << std::endl;

    ParticleFilter filter(field, "filter:config.json", state(), 10, 1);

    std::vector<double> kek = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    ParticleFilter::MistakesToProbability(kek);
    std::cout << "MistakesToProbability test ";
    double sum = 0.0;
    for(auto i : kek) {
        std::cout << i << " ";
        sum += i;
    }
    std::cout << std::endl;
    std::cout << "Probabiity sum " << sum << std::endl;
    return 0;
}