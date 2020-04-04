#pragma once
#include<stdlib.h>
#include<string>
#include<set>
#include<random>
#include <list> 

#include<env.hpp>
#include<person.hpp>

class Region {
  public:
    Region(int people_num, int processor, int P);

    void movePeople();

    void updateStatus(std::default_random_engine generator);

    void getPeopleBorder(int border);

    Person *getRandomPerson();
     
    void print();

  private:
    float world_size_ = 100.0f;
    std::set<Person> people_;
    int p_;
    int P_;
    
    //std::set<Person> inactivePeople_;
    std::uniform_real_distribution<float> UniformDistribution =
                  std::uniform_real_distribution<float>(0.0, 1.0);
};