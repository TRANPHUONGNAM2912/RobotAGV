#ifndef ROBOT_H
#define ROBOT_H

#include "Config.h"
#include "Motor.h"
#include "Encoder.h"
#include "PIDController.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Robot {
private:
    // Components
    Motor* leftMotor;
    Motor* rightMotor;
    PIDController* leftPID;
    PIDController* rightPID;
    LiquidCrystal_I2C* lcd;
    
    // Robot state
    float x;
    float y;
    float theta;
    float linearVelocity;
    float angularVelocity;
    
    // Trajectory data
    float trajectoryX[MAX_TRAJECTORY_POINTS];
    float trajectoryY[MAX_TRAJECTORY_POINTS];
    float trajectoryTheta[MAX_TRAJECTORY_POINTS];
    int trajectoryIndex;
    int currentTrajectoryPoint;
    
    // Control flags
    bool isRunning;
    bool isRecording;
    
    // Previous values for calculations
    float prevX;
    float prevY;
    float prevTheta;
    unsigned long prevTime;
    
public:
    Robot() : 
        x(0), y(0), theta(0), linearVelocity(0), angularVelocity(0),
        trajectoryIndex(0), currentTrajectoryPoint(0),
        isRunning(false), isRecording(false),
        prevX(0), prevY(0), prevTheta(0), prevTime(0) {
        
        // Initialize components
        leftMotor = new Motor(MOTOR1_PIN1, MOTOR1_PIN2, MOTOR1_ENABLE, PWM_CHANNEL_1);
        rightMotor = new Motor(MOTOR2_PIN1, MOTOR2_PIN2, MOTOR2_ENABLE, PWM_CHANNEL_2);
        leftPID = new PIDController();
        rightPID = new PIDController();
        lcd = new LiquidCrystal_I2C(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
        
        // Initialize LCD
        lcd->init();
        lcd->backlight();
    }
    
    ~Robot() {
        delete leftMotor;
        delete rightMotor;
        delete leftPID;
        delete rightPID;
        delete lcd;
    }
    
    void init() {
        // Additional initialization if needed
        reset();
    }
    
    void reset() {
        x = 0;
        y = 0;
        theta = 0;
        linearVelocity = 0;
        angularVelocity = 0;
        trajectoryIndex = 0;
        currentTrajectoryPoint = 0;
        isRunning = false;
        isRecording = false;
        
        leftPID->reset();
        rightPID->reset();
        
        // Reset encoders
        encoder1->reset();
        encoder2->reset();
        
        // Clear trajectory data
        clearTrajectory();
    }
    
    void clearTrajectory() {
        for (int i = 0; i < MAX_TRAJECTORY_POINTS; i++) {
            trajectoryX[i] = 0;
            trajectoryY[i] = 0;
            trajectoryTheta[i] = 0;
        }
        trajectoryIndex = 0;
        currentTrajectoryPoint = 0;
    }
    
    void updatePosition() {
        unsigned long currentTime = millis();
        float deltaTime = (currentTime - prevTime) / 1000.0;
        
        if (prevTime == 0 || deltaTime <= 0) {
            prevTime = currentTime;
            return;
        }
        
        // Get wheel velocities from encoders
        float leftWheelVelocity = encoder1->getDistance() / deltaTime;
        float rightWheelVelocity = encoder2->getDistance() / deltaTime;
        
        // Reset encoder counts for next calculation
        encoder1->reset();
        encoder2->reset();
        
        // Calculate robot velocities
        linearVelocity = (rightWheelVelocity + leftWheelVelocity) / 2.0;
        angularVelocity = (leftWheelVelocity - rightWheelVelocity) / WHEEL_DISTANCE;
        
        // Update position and orientation
        theta += angularVelocity * deltaTime;
        x += linearVelocity * cos(theta + (angularVelocity * deltaTime / 2.0)) * deltaTime;
        y += linearVelocity * sin(theta + (angularVelocity * deltaTime / 2.0)) * deltaTime;
        
        prevTime = currentTime;
        
        // Save trajectory point if recording
        if (isRecording && (theta != prevTheta || x != prevX || y != prevY)) {
            saveTrajectoryPoint();
        }
        
        // Update previous values
        prevX = x;
        prevY = y;
        prevTheta = theta;
    }
    
    void saveTrajectoryPoint() {
        if (trajectoryIndex < MAX_TRAJECTORY_POINTS) {
            trajectoryX[trajectoryIndex] = x;
            trajectoryY[trajectoryIndex] = y;
            trajectoryTheta[trajectoryIndex] = theta;
            
            Serial.print("Index ");
            Serial.print(trajectoryIndex);
            Serial.print(": theta = ");
            Serial.print(theta);
            Serial.print(", x = ");
            Serial.print(x);
            Serial.print(", y = ");
            Serial.println(y);
            
            trajectoryIndex++;
        }
    }
    
    void followTrajectory() {
        if (!isRunning || trajectoryIndex == 0 || currentTrajectoryPoint >= trajectoryIndex) {
            stop();
            return;
        }
        
        // Get target point from trajectory
        float targetX = trajectoryX[currentTrajectoryPoint];
        float targetY = trajectoryY[currentTrajectoryPoint];
        float targetTheta = trajectoryTheta[currentTrajectoryPoint];
        
        // Calculate errors in robot's coordinate frame
        float errorX = targetX - x;
        float errorY = targetY - y;
        float errorTheta = targetTheta - theta;
        
        // Transform errors to robot frame
        float errorXRobot = cos(theta) * errorX + sin(theta) * errorY;
        float errorYRobot = -sin(theta) * errorX + cos(theta) * errorY;
        
        // Calculate control inputs
        float linearVelocityCommand = 2.0 * errorXRobot;
        float angularVelocityCommand = 2.0 * errorYRobot + 2.0 * errorTheta;
        
        // Calculate wheel velocities
        float rightWheelVelocity = (2 * linearVelocityCommand + (WHEEL_DISTANCE * angularVelocityCommand)) / 2.0;
        float leftWheelVelocity = 2 * linearVelocityCommand - rightWheelVelocity;
        
        // Apply wheel velocities through PID controllers
        float leftRPM = abs((leftWheelVelocity / (2 * PI * WHEEL_RADIUS)) * 60);
        float rightRPM = abs((rightWheelVelocity / (2 * PI * WHEEL_RADIUS)) * 60);
        
        // Get current wheel velocities
        float currentLeftRPM = (encoder1->getCount() / ENCODER_TICKS_PER_REV) * 60;
        float currentRightRPM = (encoder2->getCount() / ENCODER_TICKS_PER_REV) * 60;
        
        // Compute PID outputs
        float leftOutput = leftPID->compute(leftRPM, currentLeftRPM);
        float rightOutput = rightPID->compute(rightRPM, currentRightRPM);
        
        // Set motor speeds with direction
        leftMotor->setSpeed(leftWheelVelocity > 0 ? leftOutput / PID_MAX_OUTPUT : -leftOutput / PID_MAX_OUTPUT);
        rightMotor->setSpeed(rightWheelVelocity > 0 ? rightOutput / PID_MAX_OUTPUT : -rightOutput / PID_MAX_OUTPUT);
        
        // Debug output
        Serial.println(targetX);
        Serial.println(targetY);
        Serial.println(targetTheta);
        Serial.println(x);
        Serial.println(y);
        Serial.println(theta);
        Serial.println(currentTrajectoryPoint);
        Serial.println(trajectoryIndex);
        Serial.println(linearVelocityCommand);
        Serial.println(angularVelocityCommand);
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");
        Serial.println("    ");
        
        // Move to next point if close enough to current target
        float distance = sqrt(errorX * errorX + errorY * errorY);
        if (distance < 0.05 && abs(errorTheta) < 0.1) {
            currentTrajectoryPoint++;
        }
    }
    
    // Control methods
    void start() {
        isRunning = true;
        currentTrajectoryPoint = 0;
    }
    
    void stop() {
        isRunning = false;
        leftMotor->stop();
        rightMotor->stop();
    }
    
    void startRecording() {
        isRecording = true;
    }
    
    void stopRecording() {
        isRecording = false;
    }
    
    void moveForward() {
        leftMotor->setSpeed(DEFAULT_LINEAR_VELOCITY);
        rightMotor->setSpeed(DEFAULT_LINEAR_VELOCITY);
    }
    
    void moveBackward() {
        leftMotor->setSpeed(-DEFAULT_LINEAR_VELOCITY);
        rightMotor->setSpeed(-DEFAULT_LINEAR_VELOCITY);
    }
    
    void turnLeft() {
        leftMotor->setSpeed(DEFAULT_LINEAR_VELOCITY);
        rightMotor->setSpeed(DEFAULT_LINEAR_VELOCITY * 1.8); // 0.27/0.15 = 1.8
    }
    
    void turnRight() {
        leftMotor->setSpeed(DEFAULT_LINEAR_VELOCITY * 1.8);
        rightMotor->setSpeed(DEFAULT_LINEAR_VELOCITY);
    }
    
    void displayStatus() {
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("X:");
        lcd->print(x * 100, 1); // Convert to cm
        lcd->print(" Y:");
        lcd->print(y * 100, 1); // Convert to cm
        
        lcd->setCursor(0, 1);
        lcd->print("Th:");
        lcd->print(theta * 180 / PI, 1); // Convert to degrees
        lcd->print(" Pts:");
        lcd->print(trajectoryIndex);
    }
    
    // Getters and setters
    bool isRobotRunning() const {
        return isRunning;
    }
    
    bool isRobotRecording() const {
        return isRecording;
    }
    
    int getTrajectoryCount() const {
        return trajectoryIndex;
    }
    
    void update() {
        updatePosition();
        
        if (isRunning) {
            followTrajectory();
        }
    }
};

#endif // ROBOT_H 