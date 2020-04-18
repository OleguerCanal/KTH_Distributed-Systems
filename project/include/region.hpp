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
    Region(int people_num, int processor, int P, std::default_random_engine *generator);

    void movePeople(std::default_random_engine *generator,
                    std::list<Person> *people_to_prev_region,
                    std::list<Person> *people_to_next_region);

    void addPeople(std::vector<Person> new_people);

    bool updateStatus(std::default_random_engine *generator);

    std::string getStatus();

    void getPeopleBorder(int border);

    void deleteSidePeople();

    Person *getRandomPerson();
     
    std::string get_serialized_people();
    
    
    void print();

  private:
    std::vector<Person> people_;
    int p_;
    int P_;
    
    //std::set<Person> inactivePeople_;
    std::uniform_real_distribution<float> UniformDistribution =
                  std::uniform_real_distribution<float>(0.0, 1.0);
};