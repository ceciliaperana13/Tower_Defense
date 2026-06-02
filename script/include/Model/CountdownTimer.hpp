#pragma once
#include <SFML/System.hpp>

class CountdownTimer {
public:
    CountdownTimer(float durationSec = 100.f);

    void setDuration(float sec);
    void start();
    float remaining() const;
    bool isFinished();

private:
    float duration;
    sf::Clock clock;
    bool finished;
};
