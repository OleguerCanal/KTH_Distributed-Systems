#pragma once
#include <math.h>
#include <string>
#include <stdlib.h>
#include <sstream>
namespace env {
    // User-defined
    extern float world_size_;
    extern int processors_in_x_direction;
    extern int number_of_people; // per region

    // Constant values
    const float TIME_STEP = 0.00001;
    const int nrDays = 10;
    const float INFECTION_RATE = 0.00005;// 0.00008;
    const float SPEED = 10;
    const float infection_distance_ = 0.04;
    const float infection_distance_squared_ = infection_distance_ * infection_distance_;

    const std::gamma_distribution<float> recovery_time_distrib = std::gamma_distribution<float>(10.0, 2.0 / 3.0);
    const std::normal_distribution<float> movement_distrib = std::normal_distribution<float>(0.0, env::TIME_STEP);

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
            px = p % env::processors_in_x_direction;
            py = p / env::processors_in_x_direction;
            Px = env::processors_in_x_direction;
            Py = P / env::processors_in_x_direction;

            color = "red";
            if ((px + py)%2 == 0) color = "black";

            bound.left = (env::world_size_ / (float)(Px)) * (float)(px);
            bound.right = (env::world_size_ / (float)(Px)) * (float)((px + 1));
            bound.upper = (env::world_size_ / (float)(Py)) * (float)((py + 1));
            bound.lower = (env::world_size_ / (float)(Py)) * (float)((py));
        }

        int get_right_region_processor() {
            return get_processor((px+1)%Px, py%Py);
        }

        int get_left_region_processor() {
            return get_processor((px+Px-1)%Px, py);  // I add P to make it cyclic
        }

        int get_above_region_processor() {
            return get_processor(px, (py+Py-1)%Py);  // I add P to make it cyclic
        }

        int get_below_region_processor() {
            return get_processor(px, (py+1)%Py);
        }

        std::string get_info() {
            std::stringstream msg;
            msg << "px:" << px << ", py:" << py << " (" << "p:" << p << ", P:" << P << ", Px:" << Px << ", Py:" << Py << ") " << color;
            return msg.str();
        }
    
      private:
        int get_processor(int x, int y) {
            return x + env::processors_in_x_direction * y;
        }
        int get_processor() {
            return px + env::processors_in_x_direction * py;
        }
    };
}