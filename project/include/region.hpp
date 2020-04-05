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
    Region(int people_num, int processor, int P);

    void movePeople();

    bool updateStatus(std::default_random_engine generator);

    std::string getStatus();

    void getPeopleBorder(int border);

    Person *getRandomPerson();
     
    void print();

  private:
    float world_size_ = 10.0f;
    std::vector<Person> people_;
    int p_;
    int P_;
    int removedPeople_ = 0;
    
    //std::set<Person> inactivePeople_;
    std::uniform_real_distribution<float> UniformDistribution =
                  std::uniform_real_distribution<float>(0.0, 1.0);
};