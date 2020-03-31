#pragma once
#include<stdlib.h>
#include<string>
#include<random>

class Person {
  public:
    float x, y;
    static const int TIME_STEP = 0.01;
    static const int INFECTION_RATE = 0.2 / TIME_STEP;
    static const std::gamma_distribution<double> GammaDistribution(5.0, 2.0 / 3.0);
    static const std::normal_distribution<double> NormalDistribution(0.0, TIME_STEP);

    Person(){}
    Person(std::string serialized_person){}
    ~Person(){}

    void move(std::default_random_engine generator);

    void infect(std::default_random_engine generator);

    int beSick();

    double getXCoord();

    double getYCoord();

    bool isInfected();

    bool isSusceptible();

    std::string serialize();

    bool operator<(const Person& other) const {
      return x < other.x;
    }


  private:
    int status_;  // -1: Susceptible, 0: Recovered, >0 Days until recovery

};