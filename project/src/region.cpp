#include <region.hpp>

Region::Region(int people_num, env::boundary* bound, std::default_random_engine *generator) {
    Region::boundary = bound;
    // Instantiate all the people
    for (int i = 0; i < people_num; i++) {
        float pos_x = (boundary->right - boundary->left) * UniformDistribution(*generator) + boundary->left;
        float pos_y = (boundary->upper - boundary->lower) * UniformDistribution(*generator) + boundary->lower;
        people_.emplace_back(Person(pos_x, pos_y));
    }
    std::sort(people_.begin(), people_.end());
}

void Region::movePeople(std::default_random_engine *generator,
                        std::list<Person> *people_to_prev_region,
                        std::list<Person> *people_to_next_region,
                        std::list<Person>* people_to_above_region,
                        std::list<Person>* people_to_below_region) {
    std::vector<Person> people_that_stay;
    people_that_stay.reserve(people_.size());  // Avoid relocation O(n)
    int region_change[] = { 0,0 };
    for (Person& person : people_) {  // O(n)
        person.move(generator,boundary, region_change);

        //Diagonal should only send to one

        if (region_change[0] >= -1 && region_change[0] <= 1 && region_change[1] >= -1 && region_change[1] <= 1)
            people_that_stay.push_back(person);
        if (region_change[0] <= -1)
            people_to_prev_region->push_back(person);
        if (region_change[0] >= 1)
            people_to_next_region->push_back(person);
        if (region_change[1] <= -1)
            people_to_above_region->push_back(person);
        if (region_change[1] >= 1)
            people_to_below_region->push_back(person);
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
    int nbSusceptibles = 0;
    for (Person person : people_) {
        if (person.isInfected())
            nbInfected++;
        if (person.isSusceptible())
            nbSusceptibles++;
    }
    msg << "S:" << nbSusceptibles << " I:" << nbInfected << " R:" << people_.size()-nbInfected-nbSusceptibles;
    return msg.str();
}

bool Region::deleteSidePeople() {
    std::vector<Person> people_that_stay;
    int nbPeople = people_.size();
    for (Person& person : people_)
        if (person.x > boundary->left && person.x < boundary->right && person.y < boundary->upper && person.y > boundary->lower)
            people_that_stay.push_back(person);
    people_ = people_that_stay;
    return (nbPeople != people_.size());
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