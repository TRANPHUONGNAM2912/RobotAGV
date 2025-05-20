#ifndef ENCODER_H
#define ENCODER_H

#include "Config.h"
#include <Arduino.h>

class Encoder {
private:
    int pinA;
    int pinB;
    volatile long count;
    volatile long direction;
    
public:
    Encoder(int pinA, int pinB) : pinA(pinA), pinB(pinB), count(0), direction(0) {
        pinMode(pinA, INPUT);
        pinMode(pinB, INPUT);
    }
    
    void reset() {
        count = 0;
        direction = 0;
    }
    
    long getCount() const {
        return count;
    }
    
    long getDirection() const {
        return direction;
    }
    
    void incrementCount() {
        count++;
        if (digitalRead(pinB) == HIGH) {
            direction--;
        } else {
            direction++;
        }
    }
    
    float getDistance() const {
        return (direction / ENCODER_TICKS_PER_REV) * (2 * PI * WHEEL_RADIUS);
    }
    
    float getVelocity(float deltaTime) const {
        return getDistance() / deltaTime;
    }
};

// Global encoder instances (needed for interrupt handlers)
extern Encoder* encoder1;
extern Encoder* encoder2;

// Interrupt handlers
void IRAM_ATTR encoder1ISR();
void IRAM_ATTR encoder2ISR();

#endif // ENCODER_H 