#include<person.hpp>

void Person::move(std::default_random_engine generator) {
    x += NormalDistribution(generator);
    y += NormalDistribution(generator);
}

void Person::infect(std::default_random_engine generator) {
    status_ = (int) (TIME_STEP * GammaDistribution(generator) + 1);
}

int Person::beSick() {
    // Pass time of being sick
    status_ -= 1;
    return status_;
}

double Person::getXCoord() {
    return x;
}

double Person::getYCoord() {
    return y;
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