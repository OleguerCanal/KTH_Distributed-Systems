#include <region.hpp>

Region::Region(int people_num, int processor, int P) {
    p_ = processor;
    P_ = P;
    // Instantiate all the people
    std::default_random_engine generator(time(0) + p_*1000);
    for (int i = 0; i < people_num; i++) {
        float pos_x = world_size_ * UniformDistribution(generator);
        float pos_y = world_size_ * UniformDistribution(generator);
        people_.emplace(Person(pos_x, pos_y));
    }
}

void Region::movePeople() {
    std::default_random_engine generator(time(0) + p_*1000);
    for (Person person : people_) {
        person.move(generator);
    }
}

void Region::updateStatus() {
    for (Person person : people_) {
        if (person.isInfected()) {
            if (person.beSick() == 0) {
                people_.erase(person);  //Change list to inactive
                //inactivePeople_.insert(person);
            } else {
                infectPeople(person);
            }
        }
    }
}

void Region::infectPeople(Person person) {
    std::default_random_engine generator(time(0) + p_*1000);
    std::set<Person> InfectablePeople = getInfectablePeople(person);
    for (Person person : InfectablePeople) {
        if (UniformDistribution(generator) < env::INFECTION_RATE)
            person.getInfected(generator);
    }
}

std::set<Person> Region::getInfectablePeople(Person person) {
    //todo: find all people close enough to be infected by the given person
}

void Region::getPeopleBorder(int border) {
}

void Region::print() {
    for (Person person : people_) {
        person.print();
    }
}