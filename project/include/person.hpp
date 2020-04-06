#pragma once
#include <iostream>
#include<stdlib.h>
#include<string>
#include<random>
#include<env.hpp>
#include <sstream>

class Person {
  public:
    float x, y;
    int status_ = -1;  // -1: Susceptible, 0: Recovered, >0 Days until recovery
    
    Person(float pos_x, float pos_y);
    Person(float pos_x, float pos_y, int status);
    ~Person(){}

    int move(std::default_random_engine *generator, int p);

    float distanceSquaredTo(Person other);

    bool tryToInfect(std::default_random_engine *generator);

    void getInfected(std::default_random_engine *generator);

    int beSick();

    bool isInfected();

    bool isSusceptible();

    std::string serialize();

    void print();

    bool operator<(const Person& other) const {
      return x < other.x;
    }

  private:
    std::gamma_distribution<float> GammaDistribution;
    std::normal_distribution<float> NormalDistribution;
    std::uniform_real_distribution<float> UniformDistribution;
};