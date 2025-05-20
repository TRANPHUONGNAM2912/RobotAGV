#include "Config.h"
#include "Motor.h"
#include "Encoder.h"
#include "PIDController.h"
#include "Robot.h"
#include "WebServer.h"
#include <Wire.h>

// Global objects
Robot* robot = nullptr;
WebServerController* webServer = nullptr;

// Timer interrupt
hw_timer_t* timer = nullptr;
volatile unsigned long timerCount = 0;

void IRAM_ATTR onTimer() {
    timerCount++;
}

// Button interrupt handler
void IRAM_ATTR stopButtonHandler() {
    if (robot) {
        robot->stop();
    }
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("Initializing RobotAGV...");
    
    // Initialize I2C for LCD
    Wire.begin();
    
    // Create robot instance
    robot = new Robot();
    
    // Create web server instance
    webServer = new WebServerController(robot);
    
    // Initialize encoders
    encoder1 = new Encoder(ENCODER1_A, ENCODER1_B);
    encoder2 = new Encoder(ENCODER2_A, ENCODER2_B);
    
    // Set up button interrupts
    pinMode(BUTTON_START, INPUT_PULLUP);
    pinMode(BUTTON_STOP, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_STOP), stopButtonHandler, FALLING);
    
    // Set up encoder interrupts
    attachInterrupt(digitalPinToInterrupt(ENCODER1_A), encoder1ISR, RISING);
    attachInterrupt(digitalPinToInterrupt(ENCODER2_A), encoder2ISR, RISING);
    
    // Set up timer interrupt
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, TIMER_INTERVAL_US, true);
    timerAlarmEnable(timer);
    
    // Initialize robot and web server
    robot->init();
    webServer->init();
    
    Serial.println("RobotAGV initialization complete");
}

void loop() {
    // Handle web server requests
    webServer->handleClient();
    
    // Check start button
    static bool prevStartButtonState = HIGH;
    bool currentStartButtonState = digitalRead(BUTTON_START);
    if (currentStartButtonState == LOW && prevStartButtonState == HIGH) {
        robot->start();
    }
    prevStartButtonState = currentStartButtonState;
    
    // Update robot state
    static unsigned long lastUpdateTime = 0;
    static unsigned long lastDisplayTime = 0;
    static unsigned long prevTimerCount = 0;
    
    // Update robot position at regular intervals
    if (timerCount != prevTimerCount) {
        robot->update();
        prevTimerCount = timerCount;
        
        // Display status every second
        unsigned long currentTime = millis();
        if (currentTime - lastDisplayTime >= 1000) {
            robot->displayStatus();
            lastDisplayTime = currentTime;
        }
    }
} 