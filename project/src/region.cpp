#include <region.hpp>

Region::Region(int people_num, int processor, int P, std::default_random_engine *generator) {
    p_ = processor;
    P_ = P;
    // Instantiate all the people
    for (int i = 0; i < people_num; i++) {
        float pos_x = env::world_size_ * UniformDistribution(*generator)+processor*env::world_size_;
        float pos_y = env::world_size_ * UniformDistribution(*generator);
        people_.emplace_back(Person(pos_x, pos_y));
    }
    std::sort(people_.begin(), people_.end());
}

void Region::movePeople(std::default_random_engine *generator,
                        std::list<Person> *people_to_prev_region,
                        std::list<Person> *people_to_next_region) {
    std::vector<Person> people_that_stay;
    people_that_stay.reserve(people_.size());  // Avoid relocation O(n)
    for (Person& person : people_) {  // O(n)
        int region_change = person.move(generator,p_);
        if (region_change >= -1 && region_change <= 1)
            people_that_stay.push_back(person);
        if (region_change <= -1) 
            people_to_prev_region->push_back(person);
        if (region_change >= 1)
            people_to_next_region->push_back(person);
    }
    people_ = people_that_stay; // Unsorted 
    // Better not to sort it yet because we still need to receive from other regions
    // std::sort(people_.begin(), people_.end());
}

void Region::addPeople(std::vector<Person> new_people) {
    people_.reserve(new_people.size());
    for (Person& person : new_people) {
        people_.push_back(person);
    }    
    std::sort(people_.begin(), people_.end()); // O(n log n)

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

void Region::deleteSidePeople() {
    std::vector<Person> people_that_stay;
    for (Person& person : people_)
        if (person.x < env::world_size_+ p_*env::world_size_ && person.x > p_ * env::world_size_)
            people_that_stay.push_back(person);
    people_ = people_that_stay;
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
        msg << person.x << "," << person.y << "," << 2*person.isInfected()+(!(person.isInfected() || person.isSusceptible())) << ";";
    }
    msg << "|";
    return msg.str();
}