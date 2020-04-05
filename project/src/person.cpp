#include<person.hpp>

Person::Person(float pos_x, float pos_y) {
    GammaDistribution = std::gamma_distribution<float>(5.0, 2.0 / 3.0);
    NormalDistribution = std::normal_distribution<float>(0.0, env::TIME_STEP);
    UniformDistribution = std::uniform_real_distribution<float>(0.0, 1.0);
    x = pos_x;
    y = pos_y;
}

void Person::move(std::default_random_engine *generator) {
    x += NormalDistribution(*generator);
    y += NormalDistribution(*generator);
    //TOREMOVE:
    if (x > 10.0)
        x -= 10.0;
    if (y > 10.0)
        y -= 10.0;
    if (x < 0.0)
        x += 10.0;
    if (y < 0.0)
        x += 10.0;
}

float Person::distanceSquaredTo(Person other) {
    return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
}

bool Person::tryToInfect(std::default_random_engine *generator) {
    if (UniformDistribution(*generator) < env::INFECTION_RATE) {
        getInfected(generator);
        return true;
    }
    return false;
}

void Person::getInfected(std::default_random_engine *generator) {
    // NOTE Isnt this very low?? 0.01*5? We'll see in simulations ;)
    status_ = (int) (1/env::TIME_STEP * GammaDistribution(*generator));
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
    std::stringstream msg;
    msg << status_ << " @ (" << x << ", " << y << ")" << std::endl;
    std::cout << msg.str();
}