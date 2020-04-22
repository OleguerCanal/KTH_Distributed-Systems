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
    std::vector<Person> people_that_stay;
    people_that_stay.reserve(people_.size());  // Avoid relocation O(n)
    for (Person& person : people_) {  // O(n)
        person.move(generator);

        if (person.x < boundary->right && person.x >= boundary->left &&
            person.y < boundary->upper && person.y >= boundary->lower) {
            people_that_stay.push_back(person);
            if (person.isInfected()) {
                if (person.x <= boundary->left + env::infection_distance_)
                    people_to_prev_region_infectious->push_back(person);
                if (person.x > boundary->right - env::infection_distance_)
                    people_to_next_region_infectious->push_back(person);
                if (person.y <= boundary->lower + env::infection_distance_)
                    people_to_below_region_infectious->push_back(person);
                if (person.y > boundary->upper - env::infection_distance_)
                    people_to_above_region_infectious->push_back(person);
            }
        }
        else {
            border_people->push_back(person);
            if (person.x < boundary->left)
                people_to_prev_region->push_back(person);
            if (person.x >= boundary->right)
                people_to_next_region->push_back(person);
            if (person.y < boundary->lower)
                if (person.x < boundary->left)
                    people_to_below_region_infectious->push_back(person);
                else if (person.x >= boundary->right)
                    people_to_below_region_infectious->push_back(person);
                else
                    people_to_below_region->push_back(person);
            if (person.y >= boundary->upper)
                if (person.x < boundary->left)
                    people_to_above_region_infectious->push_back(person);
                else if (person.x >= boundary->right)
                    people_to_above_region_infectious->push_back(person);
                else
                    people_to_above_region->push_back(person);
        }
    }
    people_ = people_that_stay; // Unsorted 
    // Better not to sort it yet because we still need to receive from other regions
    // std::sort(people_.begin(), people_.end());
}

void Region::addPeople(std::vector<Person> new_people) {
    // people_.reserve(new_people.size());
    for (Person& person : new_people) {
        people_.push_back(person);
    }    
    std::sort(people_.begin(), people_.end()); // O(n log n)

}

bool Region::updateStatus(std::default_random_engine *generator, std::vector<Person>* border_people) {
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
                if ((person.distanceSquaredTo(*person2) < env::infection_distance_squared_) && person2->isInfected()) {
                    bool inf = person.tryToInfect(generator);
                    if (inf) {
                        change = true;
                        break;
                    }
                }
            }
        }

        std::vector<Person>::iterator border_start = border_people->begin();
        if (person.isSusceptible()) { //Is still susceptible (don't re-infect)
            if (border_start != border_people->end()) {
                auto border_iterator = border_start;
                while (person.x > (*border_iterator).x - env::infection_distance_) {
                    if (person.distanceSquaredTo(*border_iterator) < env::infection_distance_squared_) {
                        bool inf = person.tryToInfect(generator);
                        if (inf) {
                            change = true;
                            break;
                        }
                    }
                    if (border_iterator++ == border_people->end())
                        break;
                }
            }
        }

        while (border_start != border_people->end() && (*border_start).x < person.x - env::infection_distance_)
            border_start++;
        //TODO advance pointer border_start

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
    std::cout << "DEPRECATED - SHOULDN'T USE DELETESIDEPEOPLE" << std::endl;
    std::vector<Person> people_that_stay;
    int nbPeople = people_.size();
    for (Person& person : people_)
        if (person.x >= boundary->left && person.x < boundary->right && person.y < boundary->upper && person.y >= boundary->lower)
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
    // msg << "|";
    return msg.str();
}