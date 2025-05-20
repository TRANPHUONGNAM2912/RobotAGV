#ifndef MOTOR_H
#define MOTOR_H

#include "Config.h"
#include <Arduino.h>

class Motor {
private:
    int pin1;
    int pin2;
    int enablePin;
    int pwmChannel;
    int pwmValue;
    
public:
    Motor(int pin1, int pin2, int enablePin, int pwmChannel) 
        : pin1(pin1), pin2(pin2), enablePin(enablePin), pwmChannel(pwmChannel), pwmValue(0) {
        
        pinMode(pin1, OUTPUT);
        pinMode(pin2, OUTPUT);
        pinMode(enablePin, OUTPUT);
        
        ledcSetup(pwmChannel, PWM_FREQUENCY, PWM_RESOLUTION);
        ledcAttachPin(enablePin, pwmChannel);
    }
    
    void setSpeed(float speed) {
        // Convert speed to PWM value (0-255)
        pwmValue = abs(speed) * 255;
        if (pwmValue > 255) pwmValue = 255;
        
        if (speed > 0) {
            // Forward
            if (pin1 == MOTOR1_PIN1) { // Left motor
                digitalWrite(pin1, HIGH);
                digitalWrite(pin2, LOW);
            } else { // Right motor
                digitalWrite(pin1, LOW);
                digitalWrite(pin2, HIGH);
            }
            ledcWrite(pwmChannel, pwmValue);
        } 
        else if (speed < 0) {
            // Backward
            if (pin1 == MOTOR1_PIN1) { // Left motor
                digitalWrite(pin1, LOW);
                digitalWrite(pin2, HIGH);
            } else { // Right motor
                digitalWrite(pin1, HIGH);
                digitalWrite(pin2, LOW);
            }
            ledcWrite(pwmChannel, pwmValue);
        } 
        else {
            // Stop
            digitalWrite(pin1, LOW);
            digitalWrite(pin2, LOW);
            ledcWrite(pwmChannel, 0);
        }
    }
    
    void stop() {
        setSpeed(0);
    }
};

#endif // MOTOR_H 