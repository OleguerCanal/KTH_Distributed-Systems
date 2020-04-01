#include<person.hpp>

Person::Person(float pos_x, float pos_y) {
    GammaDistribution = std::gamma_distribution<float>(5.0, 2.0 / 3.0);
    NormalDistribution = std::normal_distribution<float>(0.0, env::TIME_STEP);
    x = pos_x;
    y = pos_y;
}

void Person::move(std::default_random_engine generator) {
    x += NormalDistribution(generator);
    y += NormalDistribution(generator);
}

void Person::getInfected(std::default_random_engine generator) {
    // NOTE Isnt this very low?? 0.01*5?
    status_ = (float) (env::TIME_STEP * GammaDistribution(generator) + 1);
}

int Person::beSick() {
    // Pass time of being sick
    status_ -= 1;
    return status_;
}

bool Person::isInfected() {
    return status_ > 0;
}

bool Person::isSusceptible() {
    return status_ == -1;
}

std::string Person::serialize() {
    // TODO
    return "";
}

void Person::print() {
    std::cout << x << ", " << y << std::endl;
}