#ifndef WEB_SERVER_CONTROLLER_H
#define WEB_SERVER_CONTROLLER_H

#include "Config.h"
#include "Robot.h"
#include <WebServer.h>
#include <WiFi.h>

class WebServerController {
private:
    WebServer server;
    Robot* robot;
    
public:
    WebServerController(Robot* robot) : server(SERVER_PORT), robot(robot) {
    }
    
    void init() {
        // Setup WiFi access point
        WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
        Serial.println("Access Point initialized");
        Serial.print("SSID: ");
        Serial.println(WIFI_SSID);
        Serial.print("Access Point IP: ");
        Serial.println(WiFi.softAPIP());
        
        // Configure routes
        server.on("/", [this]() { handleRoot(); });
        server.on("/T", [this]() { handleForward(); });
        server.on("/B", [this]() { handleBackward(); });
        server.on("/L", [this]() { handleLeft(); });
        server.on("/R", [this]() { handleRight(); });
        server.on("/0", [this]() { handleStop(); });
        server.on("/1", [this]() { handleStart(); });
        server.on("/2", [this]() { handleStop(); });
        server.on("/3", [this]() { handleStartRecording(); });
        server.on("/4", [this]() { handleStopRecording(); });
        server.on("/5", [this]() { handleClearTrajectory(); });
        server.on("/6", [this]() { handleContinue(); });
        
        // Start server
        server.begin();
        Serial.println("Web server started");
    }
    
    void handleClient() {
        server.handleClient();
    }
    
private:
    void handleRoot() {
        server.send(200, "text/html", "OK");
    }
    
    void handleForward() {
        Serial.println("forward");
        robot->moveForward();
        server.send(200, "text/html", "OK");
    }
    
    void handleBackward() {
        Serial.println("backward");
        robot->moveBackward();
        server.send(200, "text/html", "OK");
    }
    
    void handleLeft() {
        Serial.println("left");
        robot->turnLeft();
        server.send(200, "text/html", "OK");
    }
    
    void handleRight() {
        Serial.println("right");
        robot->turnRight();
        server.send(200, "text/html", "OK");
    }
    
    void handleStop() {
        Serial.println("stop");
        robot->stop();
        server.send(200, "text/html", "OK");
    }
    
    void handleStart() {
        Serial.println("start");
        robot->start();
        server.send(200, "text/html", "OK");
    }
    
    void handleStartRecording() {
        Serial.println("start recording");
        robot->startRecording();
        server.send(200, "text/html", "OK");
    }
    
    void handleStopRecording() {
        Serial.println("stop recording");
        robot->stopRecording();
        server.send(200, "text/html", "OK");
    }
    
    void handleClearTrajectory() {
        Serial.println("clear trajectory");
        robot->clearTrajectory();
        server.send(200, "text/html", "OK");
    }
    
    void handleContinue() {
        Serial.println("continue");
        robot->start();
        server.send(200, "text/html", "OK");
    }
};

#endif // WEB_SERVER_CONTROLLER_H 
 