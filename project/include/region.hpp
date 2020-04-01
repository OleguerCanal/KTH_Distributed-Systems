#pragma once
#include<stdlib.h>
#include<string>
#include<set>
#include<random>

#include<env.hpp>
#include<person.hpp>

class Region {
  public:
    Region(int people_num, int processor, int P);

    void movePeople();

    void updateStatus();

    void getPeopleBorder(int border);

    void print();

  private:
    float world_size_ = 100.0f;
    std::set<Person> people_;
    int p_;
    int P_;
    
    //std::set<Person> inactivePeople_;
    std::uniform_real_distribution<float> UniformDistribution =
                  std::uniform_real_distribution<float>(0.0, 1.0);


    void infectPeople(Person person);

    std::set<Person> getInfectablePeople(Person person);

};