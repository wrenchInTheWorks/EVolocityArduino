#include "EVolocityChassisController.h"

EVolocityChassisController::EVolocityChassisController()
    : _throttle(0), _brake(0), _steering(0) {}

void EVolocityChassisController::begin() {
    // TODO: initialise pins / serial / CAN bus
}

void EVolocityChassisController::setThrottle(int value) {
    _throttle = constrain(value, 0, 100);
    // TODO: write _throttle to hardware
}

void EVolocityChassisController::setBrake(int value) {
    _brake = constrain(value, 0, 100);
    // TODO: write _brake to hardware
}

void EVolocityChassisController::setSteering(int value) {
    _steering = constrain(value, -100, 100);
    // TODO: write _steering to hardware
}

void EVolocityChassisController::stop() {
    setThrottle(0);
    setBrake(100);
}

int EVolocityChassisController::getSpeed() {
    // TODO: read speed from hardware / encoder
    return 0;
}
