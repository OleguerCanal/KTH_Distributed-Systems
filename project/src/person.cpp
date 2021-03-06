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

void Person::move(std::default_random_engine* generator) {
    x += NormalDistribution(*generator);
    y += NormalDistribution(*generator);
}

float Person::distanceSquaredTo(Person other) {
    return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
}

// 'Runs the dice' to see if the person gets infected and if yes, does so.
// To be called when to persons are close and one is infected and the other susceptible
bool Person::tryToInfect(std::default_random_engine *generator) {
    if (UniformDistribution(*generator) < env::INFECTION_RATE) {
        getInfected(generator);
        return true;
    }
    return false;
}

// Changes status to infected and computes for how long the person will be infected
void Person::getInfected(std::default_random_engine *generator) {
    // NOTE Isnt this very low?? 0.01*5? We'll see in simulations ;)
    status_ = (int) (1/env::TIME_STEP * GammaDistribution(*generator));
    //std::cout << status_*env::TIME_STEP << std::endl;
}

// Counts down to being healed
int Person::beSick() {
    status_ -= 1;
    return status_;
}

bool Person::isInfected() {
    return status_ > 0;
}

bool Person::isSusceptible() {
    return status_ == -1;
}

void Person::print() {
    std::stringstream msg;
    msg << status_ << " @ (" << x << ", " << y << ")" << std::endl;
    std::cout << msg.str();
}