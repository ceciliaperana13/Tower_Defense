#pragma once
#include <SFML/System.hpp>

class CountdownTimer {
public:
    CountdownTimer(float durationSec);

    void setDuration(float sec);
    void start();

    float remaining() const;
    bool  isFinished();

    // pourmain.cpp
    void update(float dt);

private:
    float     duration;
    bool      finished;
    sf::Clock clock;
};
