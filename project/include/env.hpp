#pragma once
#include <math.h>

namespace env {
    const float TIME_STEP = 0.0001;
    const float INFECTION_RATE = 0.00005;
    const float SPEED = 500;

    const float world_size_ = 1.0f;

    const float infection_distance_ = 0.1;
    const float infection_distance_squared_ = infection_distance_ * infection_distance_;
}