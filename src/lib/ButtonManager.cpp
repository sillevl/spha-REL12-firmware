#include "ButtonManager.h"

ButtonManager::ButtonManager(EventQueue* queue, const PinName* pinnames) {
    this->queue = queue;
    for(uint8_t i = 0; i<6; i++) {
        InterruptIn* button = new InterruptIn(pinnames[i], PullUp);
        button->rise(this->queue->event(Callback<void(uint8_t)>(this, &ButtonManager::released), i));
        button->fall(this->queue->event(Callback<void(uint8_t)>(this, &ButtonManager::pressed), i));
        buttons[i] = button;
    };
    timer.start();
}

void ButtonManager::onStateChange(mbed::Callback<void(uint8_t buttonIndex, ButtonState state, uint32_t time)> callback){
    stateChanged = callback;
}

void ButtonManager::pressed(uint8_t button) {
    uint32_t now = currentTimeStamp();
    stateChanged(button, PRESSED, now - lastReleaseEventTimeStamp[button]);
    lastPressEventTimeStamp[button] = now;
}

void ButtonManager::released(uint8_t button) {
    uint32_t now = currentTimeStamp();
    stateChanged(button, RELEASED, now - lastPressEventTimeStamp[button]);
    lastReleaseEventTimeStamp[button] = now;
}

uint32_t ButtonManager::currentTimeStamp() {
    return duration_cast<milliseconds>(timer.elapsed_time()).count();
}
