#pragma once
#include <math.h>

namespace env {
    const float TIME_STEP = 0.01;
    const float INFECTION_RATE = 0.7;

    const float infection_distance_ = 0.1f;
    const float infection_distance_squared_ = infection_distance_ * infection_distance_;
}