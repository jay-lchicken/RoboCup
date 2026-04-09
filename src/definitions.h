#ifndef DEF_H
#define DEF_H
#include <Arduino.h>
#include <Evo.h>

int linputmin = 80;
int linputmax = 336;
int rinputmin = 69;
int rinputmax = 286;
int outputMin = -20;
int outputMax = 300;
OTAHandler ota(AP_MODE, "EvoX1-OTA", "ROBOCUP", "12345678", "ArduinoOTAPassword");


EVOX1 evo;
EvoMotor leftMotor(M2, EVOMotor100, false);
EvoMotor rightMotor(M1, EVOMotor100, true);
EvoTriButton button(GPIO1);
EvoRGBColorSensor csleft(I2C8);
uint16_t lr, lg, lb, lc;
EvoRGBColorSensor csright(I2C7);
uint16_t rr, rg, rb, rc;
EvoTOF frontds(I2C4);
EvoTOF rightds(I2C5);
EvoMotorPair motors(&leftMotor, &rightMotor);
EvoTOF leftds(I2C6);
EvoIMU gyro(I2C1);
EvoLineLeader lineLeader(I2C3);


float rh, rs, rv; 
float lh, ls, lv;
int lcolor, rcolor;

void buttonISR() {
  esp_restart();
}

void displayValues(){
    while(true){
        evo.clearDisplay();
        evo.writeToDisplay(int(lr), 0, 0);
        evo.writeToDisplay(int(lg), 32, 0);
        evo.writeToDisplay(int(lb), 64, 0);
        evo.writeToDisplay(int(rr), 0, 8);
        evo.writeToDisplay(int(rg), 32, 8);
        evo.writeToDisplay(int(rb), 64, 8);
        // evo.writeToDisplay(int(lh), 0, 16);
        // evo.writeToDisplay(int(ls), 32, 16);
        // evo.writeToDisplay(int(lv), 64, 16);
        // evo.writeToDisplay(int(rh), 0, 24);
        // evo.writeToDisplay(int(rs), 32, 24);
        // evo.writeToDisplay(int(rv), 64, 24);
        // evo.writeToDisplay(lcolor, 0, 32);
        // evo.writeToDisplay(rcolor, 32, 32);
        evo.writeToDisplay(evo.getBattery(), 96, 0);
        evo.drawDisplay();
    }
}


void intialise(){
    // ota.begin();

    evo.begin();
    gyro.begin();
    leftMotor.begin();
    rightMotor.begin();
    leftMotor.brake();
    rightMotor.brake();
    lineLeader.begin();
    csleft.begin();
    csleft.setGain(TCS34725_GAIN_16X);
    csright.begin();
    csright.setGain(TCS34725_GAIN_16X);
    frontds.begin();
    rightds.begin();
    leftds.begin();
    delay(300);
    frontds.setHighSpeedMode();
    rightds.setHighSpeedMode();
    leftds.setHighSpeedMode();
    pinMode(GPIO3, INPUT_PULLDOWN);

    attachInterrupt(digitalPinToInterrupt(GPIO3), buttonISR, RISING);

    motors.setStartSpeed(2000);
    motors.setEndSpeed(2000);
    // displayValues();

}



#endif