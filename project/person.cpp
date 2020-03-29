#include<person.hpp>

void Person::move() {
    // TODO Move
}

void Person::infect() {
    // Set status to some random positive number
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