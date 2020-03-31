#pragma once
#include<stdlib.h>
#include<string>
#include<set>
#include<random>

#include<person.hpp>

class Region {
  public:
    int p;
    static const std::uniform_real_distribution<double> UniformDistribution(0.0, 1.0);

    Region(int people_num, int processor, int P);

    void movePeople();

    void updateStatus();
    void infectPeople(Person person);
    std::set<Person> getInfectablePeople(Person person);

    void getPeopleBorder(int border);


  private:
    float world_size_ = 100.0f;
    std::set<Person> people_;
    //std::set<Person> inactivePeople_;
};