#include<stdlib.h>
#include<string>

class Person {
  public:
    float x, y;

    Person(){}
    Person(std::string serialized_person){}
    ~Person(){}

    void move();

    void infect();

    bool isInfected();

    bool isSusceptible();

    std::string serialize();

  private:
    int status_;  // -1: Susceptible, 0: Recovered, >0 Days until recovery

};