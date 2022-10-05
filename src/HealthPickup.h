#ifndef HEALTH_PICKUP_
#define HEALTH_PICKUP_

#include "Pickup.h"

class HealthPickup : public Pickup {
    using Pickup::Pickup;

    void apply(Airplane& airplane) noexcept override {
        if (isAlive() && airplane.addHealth(1)) {
            die();
        }
    };
};

#endif