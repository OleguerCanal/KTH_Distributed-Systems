#pragma once
#include <math.h>

namespace env {
    const float TIME_STEP = 0.0001;
    const int nrDays = 30;
    const float world_size_ = 1.0f;
    const int number_of_people = 100;

    const float INFECTION_RATE = 0.00008;
    const float SPEED = 1000;
    const float infection_distance_ = 0.05;
    const float infection_distance_squared_ = infection_distance_ * infection_distance_;

    const std::gamma_distribution<float> recovery_time_distrib = std::gamma_distribution<float>(10.0, 2.0 / 3.0);
    const std::normal_distribution<float> movement_distrib = std::normal_distribution<float>(0.0, env::TIME_STEP / 100);
}