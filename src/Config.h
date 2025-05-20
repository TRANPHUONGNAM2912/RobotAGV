#ifndef CONFIG_H
#define CONFIG_H

// WiFi configuration
#define WIFI_SSID "ESP32-AP"
#define WIFI_PASSWORD "123456789"

// Robot physical parameters
#define WHEEL_DISTANCE 0.18 // Distance between wheels in meters
#define WHEEL_RADIUS 0.0325 // Wheel radius in meters (0.065/2)
#define ENCODER_TICKS_PER_REV 234.3 // Encoder ticks per revolution

// PID Controller parameters
#define PID_KP 0.02
#define PID_KI 0.00015
#define PID_KD 0.01
#define PID_MAX_OUTPUT 12.0
#define PID_MIN_OUTPUT 0.0

// Motor pins
#define MOTOR1_PIN1 2
#define MOTOR1_PIN2 4
#define MOTOR1_ENABLE 12
#define MOTOR2_PIN1 19
#define MOTOR2_PIN2 18
#define MOTOR2_ENABLE 15

// Encoder pins
#define ENCODER1_A 26
#define ENCODER1_B 13
#define ENCODER2_A 32
#define ENCODER2_B 14

// Button pins
#define BUTTON_START 25
#define BUTTON_STOP 33

// PWM configuration
#define PWM_FREQUENCY 30000
#define PWM_RESOLUTION 8
#define PWM_CHANNEL_1 0
#define PWM_CHANNEL_2 1

// LCD configuration
#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

// Timer configuration
#define TIMER_INTERVAL_US 150000 // 150ms

// Trajectory parameters
#define MAX_TRAJECTORY_POINTS 1000
#define DEFAULT_LINEAR_VELOCITY 0.15
#define DEFAULT_ANGULAR_VELOCITY 0.4

// Web server port
#define SERVER_PORT 80

#endif // CONFIG_H 