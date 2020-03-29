#pragma once
#include<stdlib.h>
#include<string>
#include<set>

#include<person.hpp>

class Region {
  public:
    static const int infection_rate;
    int p;

    Region(int people_num, int processor, int P);

    void infectPerson();

    void movePeople();

    void updateStatus();

    void getPeopleBorder(int border);

  private:
    float world_size_ = 100.0f;
    std::set<Person> people_;
};