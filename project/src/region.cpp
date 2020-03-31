#include <region.hpp>    
    
Region::Region(int people_num, int processor, int P){

}

void Region::movePeople(){
    for (const Person & person : people_) {
        person.move();
    }
}

void Region::updateStatus(){
    for (const Person & person : people_) {
        if person.isInfected() {
            if (person.beSick() == 0) {
                people_.erase(person);//Change list to inactive
                //inactivePeople_.insert(person);
            } else {
                infectPeople(person);
            }
        }
    }
}

void Region::infectPeople(Person person) {
    std::set<Person> InfectablePeople = getInfectablePeople(person);
    for (const Person & person : InfectablePeople) {
        if (UniformDistribution(generator) < INFECTION_RATE)
            person.getInfected();
    }
}

std::set<Person> Region::getInfectablePeople(Person person) {
    //todo: find all people close enough to be infected by the given person
}

void Region::getPeopleBorder(int border){

}