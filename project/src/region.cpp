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
    //TODO: resort
}

void Region::updateStatus(std::default_random_engine generator) {
    std::list<Person*> recentPeople = {};
    std::set<Person>::iterator it;
    std::list<Person*>::iterator it2;
    for (it = people_.begin(); it != people_.end(); ++it) {
        Person* person = (Person*) &(*it);

        if (person->isInfected()) {
            if (person->beSick() == 0) {
                people_.erase(*person);  //Change list to inactive
                //inactivePeople_.insert(person);
            } 
            else {
                for (it2 = recentPeople.begin(); it2 != recentPeople.end(); ++it2) {
                    Person* person2 = *it2;
                    if (person->distanceSquaredTo(*person2) < env::infection_distance_squared_ && person2->isSusceptible())
                        person2->tryToInfect(generator);
                }
            }
        } 
        else {
            for (it2 = recentPeople.begin(); it2 != recentPeople.end(); ++it2) {
                Person* person2 = *it2;
                if (person->distanceSquaredTo(*person2) < env::infection_distance_squared_ && person2->isInfected())
                    person->tryToInfect(generator);
            }
        }

        while ((recentPeople.size() > 0) && ((*recentPeople.begin())->x < person->x - env::infection_distance_))
            recentPeople.pop_front();

        recentPeople.push_back(person);
    }
}

void Region::getPeopleBorder(int border) {
    //TODO: returns all people at env::infection_distance_/2 of the border
}

Person * Region::getRandomPerson() {
    std::set<Person>::iterator it = people_.begin();
    std::advance(it, rand() % people_.size());
    return (Person*) &(*it);
}

void Region::print() {
    for (Person person : people_) {
        person.print();
    }
}