#include <region.hpp>

Region::Region(int people_num, int processor, int P, std::default_random_engine *generator) {
    p_ = processor;
    P_ = P;
    // Instantiate all the people
    for (int i = 0; i < people_num; i++) {
        float pos_x = env::world_size_ * UniformDistribution(*generator);
        float pos_y = env::world_size_ * UniformDistribution(*generator);
        people_.emplace_back(Person(pos_x, pos_y));
    }
    std::sort(people_.begin(), people_.end());
}

void Region::movePeople(std::default_random_engine *generator) {
    for (Person& person : people_) {
        person.move(generator);
    }
    std::sort(people_.begin(), people_.end());
}

bool Region::updateStatus(std::default_random_engine *generator) {
    // Returns total number of infected people
    bool change = false;
    std::list<Person*> recentPeople = {};
    for (Person& person : people_) {
        if (person.isInfected()) {
            if (person.beSick() == 0) {
                change = true;
                //people_.erase(*person);  //TODO
                removedPeople_++;
                //inactivePeople_.insert(person);
            } 
            else {
                for (Person* person2 : recentPeople) {
                    if ((person.distanceSquaredTo(*person2) < env::infection_distance_squared_) && person2->isSusceptible())
                        change |= person2->tryToInfect(generator);
                }
            }
        } 
        if (person.isSusceptible()) {
            for (Person* person2 : recentPeople) {
                if ((person.distanceSquaredTo(*person2) < env::infection_distance_squared_) && person2->isInfected())
                    change |= person.tryToInfect(generator);
            }
        }

        while ((recentPeople.size() > 0) && ((person.x - (*recentPeople.begin())->x) >  env::infection_distance_))
            recentPeople.pop_front();

        recentPeople.push_back(&person);
    }

    return change;
}

std::string Region::getStatus() {
    std::stringstream msg;
    int nbInfected = 0;
    for (Person person : people_)
        if (person.isInfected())
            nbInfected++;
    msg << "S:" << people_.size() - nbInfected - removedPeople_ << " I:" << nbInfected << " R:" << removedPeople_;
    return msg.str();
}

void Region::getPeopleBorder(int border) {
    //TODO: returns all people at env::infection_distance_/2 of the border
}

Person * Region::getRandomPerson() {
    return &people_[rand() % people_.size()];
}

void Region::print() {
    for (Person person : people_) {
        person.print();
    }
}

std::string Region::get_serialized_people() {
    std::stringstream msg;
    for (Person person : people_) {
        msg << person.x << "," << person.y << "," << person.isInfected()+2*(!(person.isInfected() || person.isSusceptible())) << ";";
    }
    msg << "|";
    return msg.str();
}