#include "CountdownTimer.hpp"

CountdownTimer::CountdownTimer(float durationSec)
    : duration(durationSec), finished(false)
{
}

void CountdownTimer::setDuration(float sec) {
    duration = sec;
}

void CountdownTimer::start() {
    clock.restart();
    finished = false;
}

float CountdownTimer::remaining() const {
    float r = duration - clock.getElapsedTime().asSeconds();
    return (r < 0.f ? 0.f : r);
}

bool CountdownTimer::isFinished() {
    if (!finished && clock.getElapsedTime().asSeconds() >= duration) {
        finished = true;
    }
    return finished;
}

void CountdownTimer::update(float dt) {
    if (finished) return;

    float r = remaining() - dt;
    if (r <= 0.f) {
        finished = true;
    }
}
