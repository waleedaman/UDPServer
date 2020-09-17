#pragma once
#include <WiFiUdp.h>
#include "Arduino.h"
class AHcode{
    String direction;
    int speed;
   // int height;
    float current;
    int brakeA= 8;//brake
    int dir1PinA = 12;//direction
    int speedPinA = 3; // PWM control
    bool check_bool= false;
    int value=0;
    int minHeight = 0;
    int maxHeight = 30;
    int minPot = 35;
    int maxPot = 944;
public:
    void init();
    float getCurrent();
    void setMotor(String dir,int speed, int height);
    int mapValue(int value);
    void readPacket(WiFiUDP Udp,char* comm);
    int getCurrentHeight();
    void moveUp(int speed);
    void moveDown(int speed);
    void stop();
};
