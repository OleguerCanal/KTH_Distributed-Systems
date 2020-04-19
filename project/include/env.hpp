#pragma once
#include <math.h>

namespace env {
    const float TIME_STEP = 0.0001;
    const int nrDays = 30;
    const float world_size_ = 1.0f;
    const int processors_in_x_direction = 2.0;
    const int number_of_people = 100; // per region

    const float INFECTION_RATE = 0.001;// 0.00008;
    const float SPEED = 1000.0;
    const float infection_distance_ = 0.05;
    const float infection_distance_squared_ = infection_distance_ * infection_distance_;

    const std::gamma_distribution<float> recovery_time_distrib = std::gamma_distribution<float>(10.0, 2.0 / 3.0);
    const std::normal_distribution<float> movement_distrib = std::normal_distribution<float>(0.0, env::TIME_STEP / 100);

    struct boundary {
        float left;
        float right;
        float upper;
        float lower;

        boundary() {
        }
    };

    struct RegionCoordinates {
      public:
        int px, py, Px, Py, p, P;
        env::boundary bound;

        RegionCoordinates(int processor, int Processors) : bound() {
            p = processor;
            P = Processors;
            px = p % env::processors_in_x_direction;
            py = p / env::processors_in_x_direction;
            Px = env::processors_in_x_direction;
            Py = P / env::processors_in_x_direction;

            bound.left = (env::world_size_ / (float)(Px)) * (float)(px);
            bound.right = (env::world_size_ / (float)(Px)) * (float)((px + 1));
            bound.upper = (env::world_size_ / (float)(Py)) * (float)((py + 1));
            bound.lower = (env::world_size_ / (float)(Py)) * (float)(py);
        }

        int get_right_region_processor() {
            return get_processor((px+1)%Px, py%Py);
        }

        int get_left_region_processor() {
            return get_processor((px-1)%Px, py);
        }

        int get_above_region_processor() {
            return get_processor(px, (py-1)%Py);
        }

        int get_below_region_processor() {
            return get_processor(px, (py+1)%Py);
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