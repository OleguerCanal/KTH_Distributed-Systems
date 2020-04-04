#pragma once

namespace env {
    const float TIME_STEP = 0.01;
    const float INFECTION_RATE = 0.2 * TIME_STEP;

    const float infection_distance_ = 30.0f;
    const float infection_distance_squared_ = infection_distance_ * infection_distance_;
}