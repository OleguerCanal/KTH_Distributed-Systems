#pragma once
#include<stdlib.h>
#include<string>
#include<set>
#include<random>
#include <list> 
#include <algorithm>

#include<env.hpp>
#include<person.hpp>

class Region {
  public:
    Region(int people_num, env::boundary* boundary, std::default_random_engine *generator);

    void movePeople(std::default_random_engine *generator,
                    std::list<Person> *people_to_prev_region,
                    std::list<Person> *people_to_next_region,
                    std::list<Person>* people_to_above_region,
                    std::list<Person>* people_to_below_region,
                    std::vector<Person>* border_people);

    void addPeople(std::vector<Person> new_people);

    bool updateStatus(std::default_random_engine *generator, std::vector<Person> *border_people);

    std::string getStatus();

    bool deleteSidePeople();

    Person *getRandomPerson();
     
    std::string get_serialized_people();
    
    
    void print();

  private:
    env::boundary* boundary;
    std::vector<Person> people_;
    
    //std::set<Person> inactivePeople_;
    std::uniform_real_distribution<float> UniformDistribution =
                  std::uniform_real_distribution<float>(0.0, 1.0);
};