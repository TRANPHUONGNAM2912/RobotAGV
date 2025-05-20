#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include "Config.h"
#include <Arduino.h>

class PIDController {
private:
    float kp;
    float ki;
    float kd;
    float maxOutput;
    float minOutput;
    
    float errorPrev;
    float integralPrev;
    unsigned long timePrev;
    
public:
    PIDController(float kp = PID_KP, float ki = PID_KI, float kd = PID_KD, 
                  float maxOutput = PID_MAX_OUTPUT, float minOutput = PID_MIN_OUTPUT)
        : kp(kp), ki(ki), kd(kd), maxOutput(maxOutput), minOutput(minOutput),
          errorPrev(0), integralPrev(0), timePrev(0) {
    }
    
    void reset() {
        errorPrev = 0;
        integralPrev = 0;
        timePrev = 0;
    }
    
    float compute(float setpoint, float processVariable) {
        unsigned long timeNow = millis();
        float dt = (timePrev > 0) ? (timeNow - timePrev) / 1000.0 : 0.01;
        
        // Calculate error
        float error = setpoint - processVariable;
        
        // Calculate integral term
        float integral = integralPrev + (dt * (error + errorPrev) / 2.0);
        
        // Calculate derivative term
        float derivative = (error - errorPrev) / dt;
        
        // Calculate output
        float output = kp * error + ki * integral + kd * derivative;
        
        // Limit output
        if (output > maxOutput) {
            output = maxOutput;
            integral = integralPrev; // Anti-windup
        } else if (output < minOutput) {
            output = minOutput;
            integral = integralPrev; // Anti-windup
        }
        
        // Save values for next iteration
        errorPrev = error;
        integralPrev = integral;
        timePrev = timeNow;
        
        return output;
    }
    
    // Getters and setters for PID parameters
    void setTunings(float kp, float ki, float kd) {
        this->kp = kp;
        this->ki = ki;
        this->kd = kd;
    }
    
    void setOutputLimits(float min, float max) {
        minOutput = min;
        maxOutput = max;
    }
};

#endif // PID_CONTROLLER_H 