#include<person.hpp>

Person::Person(float pos_x, float pos_y) {
    GammaDistribution = env::recovery_time_distrib;
    NormalDistribution = env::movement_distrib;
    UniformDistribution = std::uniform_real_distribution<float>(0.0, 1.0);
    x = pos_x;
    y = pos_y;
}

Person::Person(float pos_x, float pos_y, int status) {
    GammaDistribution = env::recovery_time_distrib;
    NormalDistribution = env::movement_distrib;
    UniformDistribution = std::uniform_real_distribution<float>(0.0, 1.0);
    x = pos_x;
    y = pos_y;
    status_ = status;
}


void Person::move(std::default_random_engine* generator, env::boundary* boundary) {
    x += NormalDistribution(*generator) * env::SPEED;
    y += NormalDistribution(*generator) * env::SPEED;
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
    //std::cout << status_*env::TIME_STEP << std::endl;
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