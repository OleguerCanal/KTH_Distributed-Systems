#pragma once
#include <math.h>
#include <string>
#include <stdlib.h>
#include <sstream>
namespace env {
    // User-defined
    extern float WORLD_SIZE;
    extern int PROCESSORS_IN_X_DIRECTION;
    extern int NR_PEOPLE; // per region

    // Constant values
    const int NR_DAYS = 200;                        // Number of days to run the simulation
    const float TIME_STEP = 0.01;                   // Discretization timestep (Check REGION_SIZE/BASE_SPEED/sqrt(TIME_STEP) > 10)
    const float BASE_SPEED = 10.0;                  // in metres (A person moves base_speed*K a day on average (K=sqrt(pi/2)=0.79788))
    const float INFECTION_RATE_PER_MINUTE = 0.1;    // Have a 0.1 chance for every minute close to an infected to get infected

    const float INFECTION_RATE = 1-pow(1 - INFECTION_RATE_PER_MINUTE,24.0*60.0*TIME_STEP);  // Make infection time step independant
    const float SPEED = BASE_SPEED *sqrt(TIME_STEP);                                        // Make movement time step independant
    const float INFECTION_DISTANCE = 1;                                                     // Infect at one meter
    const float INFECTION_DISTANCE_SQUARED = INFECTION_DISTANCE * INFECTION_DISTANCE;

    const std::gamma_distribution<float> recovery_time_distrib = std::gamma_distribution<float>(10.0, 2.0 / 3.0);   // Time till recovery
    const std::normal_distribution<float> movement_distrib = std::normal_distribution<float>(0.0, SPEED);           // Movement

    struct boundary {
        float left;
        float right;
        float upper;
        float lower;
        boundary() {}
    };

    struct RegionCoordinates {
      public:
        int px, py, Px, Py, p, P;
        std::string color;
        env::boundary bound;

        RegionCoordinates(int processor, int Processors) {
            p = processor;
            P = Processors;
            px = p % env::PROCESSORS_IN_X_DIRECTION;
            py = p / env::PROCESSORS_IN_X_DIRECTION;
            Px = env::PROCESSORS_IN_X_DIRECTION;
            Py = P / env::PROCESSORS_IN_X_DIRECTION;

            color = "red";
            if ((px + py)%2 == 0) color = "black";

            bound.left = (env::WORLD_SIZE / (float)(Px)) * (float)(px);
            bound.right = (env::WORLD_SIZE / (float)(Px)) * (float)((px + 1));
            bound.upper = (env::WORLD_SIZE / (float)(Py)) * (float)((py + 1));
            bound.lower = (env::WORLD_SIZE / (float)(Py)) * (float)((py));
        }

        int get_right_region_processor() {
            return get_processor((px+1)%Px, py%Py);
        }

        int get_left_region_processor() {
            return get_processor((px+Px-1)%Px, py);  // I add P to make it cyclic
        }

        int get_above_region_processor() {
            return get_processor(px, (py+1)%Py);  // I add P to make it cyclic
        }

        int get_below_region_processor() {
            return get_processor(px, (py+Py-1)%Py);
        }

        std::string get_info() {
            std::stringstream msg;
            msg << "px:" << px << ", py:" << py << " (" << "p:" << p << ", P:" << P << ", Px:" << Px << ", Py:" << Py << ") " << color;
            return msg.str();
        }
    
      private:
        int get_processor(int x, int y) {
            return x + env::PROCESSORS_IN_X_DIRECTION * y;
        }
        int get_processor() {
            return px + env::PROCESSORS_IN_X_DIRECTION * py;
        }
    };
}