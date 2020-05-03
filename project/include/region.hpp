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
    env::RegionCoordinates* coordinates;

    Region(int people_num, env::RegionCoordinates* coord, std::default_random_engine *generator);

    void movePeople(std::default_random_engine *generator,
                    std::list<Person> *people_to_prev_region,
                    std::list<Person> *people_to_next_region,
                    std::list<Person>* people_to_above_region,
                    std::list<Person>* people_to_below_region,
        std::list<Person>* people_to_prev_region_infectious,
        std::list<Person>* people_to_next_region_infectious,
        std::list<Person>* people_to_above_region_infectious,
        std::list<Person>* people_to_below_region_infectious,
                    std::vector<Person>* border_people);

    void addPeople(std::vector<Person> new_people);

    bool updateStatus(std::default_random_engine *generator, std::vector<Person> *border_people);

    std::string getStatus(bool human_readable=true);

    Person *getRandomPerson();
     
    std::string get_serialized_people();
    
    void print();

    std::vector<Person> people_;

  private:
    env::boundary* boundary;
    
    //std::set<Person> inactivePeople_;
    std::uniform_real_distribution<float> UniformDistribution =
                  std::uniform_real_distribution<float>(0.0, 1.0);
};