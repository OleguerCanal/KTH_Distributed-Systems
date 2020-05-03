#include <region.hpp>

Region::Region(int people_num, env::RegionCoordinates* coord, std::default_random_engine *generator) {
    coordinates = coord;
    boundary = &(coord->bound);

    // Instantiate all the people
    for (int i = 0; i < people_num; i++) {
        float pos_x = (boundary->right - boundary->left) * UniformDistribution(*generator) + boundary->left;
        float pos_y = (boundary->upper - boundary->lower) * UniformDistribution(*generator) + boundary->lower;
        people_.emplace_back(Person(pos_x, pos_y));
    }

    // Sort people
    std::sort(people_.begin(), people_.end());
}

void Region::movePeople(std::default_random_engine *generator,
                        std::list<Person> *people_to_prev_region_infectious,
                        std::list<Person> *people_to_next_region_infectious,
                        std::list<Person>* people_to_above_region_infectious,
                        std::list<Person>* people_to_below_region_infectious,
                        std::list<Person>* people_to_prev_region,
                        std::list<Person>* people_to_next_region,
                        std::list<Person>* people_to_above_region,
                        std::list<Person>* people_to_below_region,
                        std::vector<Person>* border_people) {

    // Move each person and determine whether they stay or to which processor they are being sent
    // Makes sure no person is duplicated
    // Will also determine if someone is close to the border and should be sent aswell
    std::vector<Person> people_that_stay;
    people_that_stay.reserve(people_.size());  // Avoid relocation O(n)
    for (Person& person : people_) {  // O(n)
        person.move(generator);

        if (person.x < boundary->right && person.x >= boundary->left &&
            person.y < boundary->upper && person.y >= boundary->lower) {
            people_that_stay.push_back(person);
            if (person.isInfected()) {
                if (person.x <= boundary->left + env::INFECTION_DISTANCE)
                    people_to_prev_region_infectious->push_back(person);
                if (person.x > boundary->right - env::INFECTION_DISTANCE)
                    people_to_next_region_infectious->push_back(person);
                if (person.y <= boundary->lower + env::INFECTION_DISTANCE)
                    people_to_below_region_infectious->push_back(person);
                if (person.y > boundary->upper - env::INFECTION_DISTANCE)
                    people_to_above_region_infectious->push_back(person);
            }
        }
        else {
            if (person.isInfected())
                border_people->push_back(person);
            if (person.x < boundary->left)
                people_to_prev_region->push_back(person);
            if (person.x >= boundary->right)
                people_to_next_region->push_back(person);
            if (person.y < boundary->lower)
                if (person.x < boundary->left || person.x >= boundary->right) {
                    if (person.isInfected())
                        people_to_below_region_infectious->push_back(person);
                } else
                    people_to_below_region->push_back(person);
            if (person.y >= boundary->upper)
                if (person.x < boundary->left || person.x >= boundary->right) {
                    if (person.isInfected())
                        people_to_above_region_infectious->push_back(person);
                } else
                    people_to_above_region->push_back(person);
        }
    }
    people_ = people_that_stay; // Unsorted 
    // Better not to sort it yet because we still need to receive from other regions
    // std::sort(people_.begin(), people_.end());
}

void Region::addPeople(std::vector<Person> new_people) {
    // Adds incoming people and sorts the whole array
    for (Person& person : new_people) {
        people_.push_back(person);
    }    
    std::sort(people_.begin(), people_.end()); // O(n log n)

}

bool Region::updateStatus(std::default_random_engine *generator, std::vector<Person>* border_people) {
    // Returns whether a status got updated (someone became infected or removed)
    bool change = false;
    std::list<Person*> recentPeople = {};
    std::vector<Person>::iterator border_start = border_people->begin();
    for (Person& person : people_) {
        // For infected people 
        // - be sick (count down the infected time)
        // - if a non infected person is close, infect him/her
        if (person.isInfected()) {
            if (person.beSick() == 0) {
                change = true;
            } 
            else {
                for (Person* person2 : recentPeople) {
                    if ((person.distanceSquaredTo(*person2) < env::INFECTION_DISTANCE_SQUARED) && person2->isSusceptible())
                        change |= person2->tryToInfect(generator);
                }
            }
        } 
        // For susceptible people
        // - if an infected person is close, infect yourself
        // - check with the people at the border if they can infect you
        if (person.isSusceptible()) {
            for (Person* person2 : recentPeople) {
                if ((person.distanceSquaredTo(*person2) < env::INFECTION_DISTANCE_SQUARED) && person2->isInfected()) {
                    bool inf = person.tryToInfect(generator);
                    if (inf) {
                        change = true;
                        break;
                    }
                }
            }
        }

        if (person.isSusceptible()) { //Is still susceptible (don't re-infect)
            if (border_start != border_people->end()) {
                auto border_iterator = border_start;
                while (border_iterator != border_people->end() && person.x > (*border_iterator).x - env::INFECTION_DISTANCE) {
                    if (person.distanceSquaredTo(*border_iterator) < env::INFECTION_DISTANCE_SQUARED) {
                        bool inf = person.tryToInfect(generator);
                        if (inf) {
                            change = true;
                            break;
                        }
                    }
                    border_iterator++;
                }
            }
        }
        
        // Keep only people to compare with that have an x-coordinate close enough (both for people at the border and inside the region)
        while (border_start != border_people->end() && (*border_start).x < person.x - env::INFECTION_DISTANCE)
            border_start++;

        while ((recentPeople.size() > 0) && ((person.x - (*recentPeople.begin())->x) >  env::INFECTION_DISTANCE))
            recentPeople.pop_front();

        recentPeople.push_back(&person);
    }

    return change;
}

// Returns the number of susceptibles, infected and removed
std::string Region::getStatus(bool human_readable) {
    std::stringstream msg;
    int nbInfected = 0;
    int nbSusceptibles = 0;
    for (Person person : people_) {
        if (person.isInfected())
            nbInfected++;
        if (person.isSusceptible())
            nbSusceptibles++;
    }
    if (human_readable)
        msg << "S:" << nbSusceptibles << " I:" << nbInfected << " R:" << people_.size()-nbInfected-nbSusceptibles;
    else {
        msg << nbSusceptibles << "," << nbInfected << "," << people_.size()-nbInfected-nbSusceptibles;
        msg << "|";
    }
    return msg.str();
}

// Returns a random person in the region
Person * Region::getRandomPerson() {
    return &people_[rand() % people_.size()];
}

// Print every person in the region
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
    // msg << "|";
    return msg.str();
}