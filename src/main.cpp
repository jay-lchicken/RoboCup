#include "definitions.h"
#include "linetrack.h"
#include "walltrack.h"
#include "linefinder.h"
#include "getcolor.h"

void setup() {
    intialise();
    evo.playTone(1000, 200);
    evo.selectI2CChannel(I2C2);
    // for (byte i=0; i< 127; i++){
    //   Wire.beginTransmission(i);
    //   int error = Wire.endTransmission();
    //   if (error == 0){
    //     evo.clearDisplay();
    //     evo.writeToDisplay("I2C Device Found", 0, 0);
    //     evo.writeToDisplay("Address: ", 0, 8);
    //     evo.writeToDisplay(i, 64, 8);
    //     evo.drawDisplay();
    //     delay(1000);
    //   }
    // }
    // delay(100000);

    int frontDistance = frontds.getDistance();
    int criteriaCount = 0;
    int inLoopCount = false;
    int readCount = 0;
    // color values are 0 = white, 1 = red, 2 = green, 3 = black
    if (esp_reset_reason() != ESP_RST_SW) {
        button.waitForBump(1);
        while (button.getButton(1) == 0) {
            csleft.getRawRGBC(&lr, &lg, &lb, &lc);
            csright.getRawRGBC(&rr, &rg, &rb, &rc);
            getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
            evo.clearDisplay();
            evo.writeToDisplay(int(lr), 0, 0);
            evo.writeToDisplay(int(lg), 32, 0);
            evo.writeToDisplay(int(lb), 64, 0);
            evo.writeToDisplay(int(lc), 0, 40);
            evo.writeToDisplay(int(rr), 0, 8);
            evo.writeToDisplay(int(rg), 32, 8);
            evo.writeToDisplay(int(rb), 64, 8);
            evo.writeToDisplay(int(rc), 32, 40);
            evo.writeToDisplay(int(lh), 0, 16);
            evo.writeToDisplay(int(ls), 32, 16);
            evo.writeToDisplay(int(lv), 64, 16);
            evo.writeToDisplay(int(rh), 0, 24);
            evo.writeToDisplay(int(rs), 32, 24);
            evo.writeToDisplay(int(rv), 64, 24);
            evo.writeToDisplay(lcolor, 0, 32);
            evo.writeToDisplay(rcolor, 32, 32);
            evo.writeToDisplay(leftds.getDistance(), 0, 48);
            evo.writeToDisplay(rightds.getDistance(), 32, 48);
            evo.writeToDisplay((gyro.getEulerY()), 0, 56);
            evo.writeToDisplay(evo.getBattery(), 96, 0);
            evo.writeToDisplay(4096 - lineLeader.readADC(4), 96, 8);
            evo.drawDisplay();
            csleft.getRawRGBC(&lr, &lg, &lb, &lc);
            csright.getRawRGBC(&rr, &rg, &rb, &rc);
            getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
            int leftValue = map(lc, linputmin, linputmax, outputMin, outputMax);
            int rightValue = map(rc, rinputmin, rinputmax, outputMin, outputMax);
        }
    }
    while (lcolor != 1 || rcolor != 1) //while not red
    {
        csleft.getRawRGBC(&lr, &lg, &lb, &lc);
        csright.getRawRGBC(&rr, &rg, &rb, &rc);
        getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
        if (readCount >= 20) {
            frontDistance = frontds.getDistance();
            readCount = 0;
        }
        readCount++;
        while (gyro.getEulerY() > 20 || gyro.getEulerY() < -7) {

            evo.clearDisplay();
            evo.writeToDisplay("Up/Down Ramp", 0, 0);
            evo.drawDisplay();

            // findline();
            csleft.getRawRGBC(&lr, &lg, &lb, &lc);
            csright.getRawRGBC(&rr, &rg, &rb, &rc);
            linetrack(26, 30, 10, 3300, lc, rc);
        }
        evo.clearDisplay();
        if ((lcolor == 2 && lc < 350) || (rcolor == 2 && rc < 350)) //if any sense green and that its not a super close object
        {
            motors.brake();
            evo.clearDisplay();
            evo.writeToDisplay("Green Sensed", 0, 0);
            evo.drawDisplay();
            delay(1000);
            int direction = 0; // 0 is right, 1 is left, 2 is backwards
            evo.clearDisplay();
            evo.writeToDisplay(lcolor, 0, 0);
            evo.writeToDisplay(rcolor, 0, 8);
            motors.moveDegrees(2000, 2000, 30, BRAKE);
            if (lcolor == 2 && rcolor != 2) // case 1
            {
                motors.brake();
                motors.brake();
                evo.clearDisplay();
                evo.writeToDisplay("Green, NIL", 0, 0);
                evo.drawDisplay();
                delay(1000);
                while (lcolor == 2) {
                    csleft.getRawRGBC(&lr, &lg, &lb, &lc);
                    getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
                    motors.move(2000, 2000);
                }
                motors.brake();

                // evo.clearDisplay();
                // evo.writeToDisplay(lcolor, 0, 0);
                // evo.drawDisplay();
                // delay(5000);
                if (lcolor == 3) {
                    motors.brake();
                    evo.clearDisplay();
                    evo.writeToDisplay("Sense Blackline (L)", 0, 0);
                    delay(1000);
                    evo.drawDisplay();
                    motors.moveDegrees(2000, 2000, 55, BRAKE);
                    motors.moveDegrees(-2000, 2000, 220, BRAKE);
                    motors.moveDegrees(2000, 2000, 40, BRAKE);
                } else {
                    motors.brake();
                    evo.clearDisplay();
                    evo.writeToDisplay("Did Not Sense Blackline", 0, 0);
                    evo.drawDisplay();
                    delay(1000);
                }
            }
            if (lcolor != 2 && rcolor == 2) // case 2
            {
                motors.brake();
                motors.brake();
                evo.clearDisplay();
                evo.writeToDisplay("NIL, GREEN", 0, 0);
                evo.drawDisplay();
                delay(1000);
                while (rcolor == 2) {
                    csright.getRawRGBC(&rr, &rg, &rb, &rc);
                    getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
                    motors.move(2000, 2000);
                }
                motors.brake();
                if (rcolor == 3) {
                    csright.getRawRGBC(&rr, &rg, &rb, &rc);
                    getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
                    motors.brake();
                    evo.clearDisplay();
                    evo.writeToDisplay("Sense Blackline (R)", 0, 0);
                    delay(1000);
                    evo.drawDisplay();
                    motors.moveDegrees(2000, 2000, 55, BRAKE);
                    motors.moveDegrees(2000, -2000, 220, BRAKE);
                    motors.moveDegrees(2000, 2000, 40, BRAKE);
                } else {
                    motors.brake();
                    evo.clearDisplay();
                    evo.writeToDisplay("Did Not Sense Blackline", 0, 0);
                    evo.drawDisplay();
                    delay(1000);
                }
            }
            if (lcolor == 2 && rcolor == 2) // case 3
            {
                motors.brake();
                motors.brake();
                evo.clearDisplay();
                evo.writeToDisplay("GREEN, GREEN", 0, 0);
                evo.drawDisplay();
                delay(1000);
                while (rcolor == 2) {
                    csright.getRawRGBC(&rr, &rg, &rb, &rc);
                    getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
                    motors.move(2000, 2000);
                }
                motors.brake();
                if (rcolor == 3) {
                    csright.getRawRGBC(&rr, &rg, &rb, &rc);
                    getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
                    motors.brake();
                    evo.clearDisplay();
                    evo.writeToDisplay("Sense Blackline (U)", 0, 0);
                    delay(1000);
                    evo.drawDisplay();
                    motors.moveDegrees(2000, -2000, 430, BRAKE);
                    motors.moveDegrees(2000, 2000, 80, BRAKE);
                } else {
                    motors.brake();
                    evo.clearDisplay();
                    evo.writeToDisplay("Did Not Sense Blackline", 0, 0);
                    evo.drawDisplay();
                    delay(1000);
                }
            }
        } else if ((4096 - lineLeader.readADC(2)) > 160) {
            motors.brake();
            evo.clearDisplay();
            evo.writeToDisplay("Silver Sensed", 0, 0);
            evo.drawDisplay();
            delay(1000);
            if (findlinesilver()) {
                motors.brake();
                evo.clearDisplay();
                evo.writeToDisplay("Entering Evac Zone", 0, 0);
                evo.drawDisplay();
                motors.moveDegrees(2000, 2000, 200, BRAKE);
                delay(1000);


                csright.getRawRGBC(&rr, &rg, &rb, &rc);
                csleft.getRawRGBC(&lr, &lg, &lb, &lc);
                getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
                int scene = 0;
                if (leftds.getDistance() < rightds.getDistance()) {
                    evo.clearDisplay();
                    evo.writeToDisplay("LEFT<RIGHT", 0, 32);
                    evo.drawDisplay();
                    while (lcolor != 3 && rcolor != 3) {
                        walltrackright(120, 50, 2000, 145);

                        csleft.getRawRGBC(&lr, &lg, &lb, &lc);
                        csright.getRawRGBC(&rr, &rg, &rb, &rc);

                        if (leftds.getDistance() > 250) {
                            break;
                        }

                        getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
                        evo.clearDisplay();
                        evo.writeToDisplay(lcolor, 0, 32);
                        evo.drawDisplay();
                    }
                } else {
                    scene = 1;
                    evo.clearDisplay();
                    evo.writeToDisplay("LEFT>RIGHT", 0, 32);
                    evo.drawDisplay();
                    while (lcolor != 3 && rcolor != 3) {

                        walltrackleft(120, 50, 2000, 145);
                        csleft.getRawRGBC(&rr, &rg, &rb, &rc);

                        csright.getRawRGBC(&rr, &rg, &rb, &rc);
                        int rightdsva = rightds.getDistance();
                        if (rightdsva> 250) {
                            break;
                        }

                        getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
                        evo.clearDisplay();
                        evo.writeToDisplay(rcolor, 0, 32);
                        evo.writeToDisplay("rightds: "+rightdsva, 0, 32);

                        evo.drawDisplay();
                    }
                }

                motors.brake();
                evo.clearDisplay();
                delay(2000);
                if (scene == 1) {
                    motors.moveDegrees(-2000, -2000, 80, BRAKE);
                    motors.moveDegrees(2000, -2000, 100, BRAKE);
                }else if (scene == 0){
                    motors.moveDegrees(-2000, -2000, 80, BRAKE);
                    motors.moveDegrees(-2000, 2000, 100, BRAKE);
                }
                csright.getRawRGBC(&rr, &rg, &rb, &rc);
                csleft.getRawRGBC(&lr, &lg, &lb, &lc);
                getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
            }
        } else if (frontDistance < 90 && frontDistance != 0) {
            if (!inLoopCount) {
                criteriaCount++;
                inLoopCount = true;
                evo.clearDisplay();
                evo.writeToDisplay(criteriaCount, 32,32);
                evo.drawDisplay();
            }
            if (criteriaCount > 5) {
                break;
            }
            evo.clearDisplay();
            evo.writeToDisplay("DETECTED FRONT", 32,32);
            evo.drawDisplay();
            motors.brake();
            if (leftds.getDistance() < rightds.getDistance()) {
                // turn right
                motors.moveDegrees(-4700, -4700, 120, BRAKE);
                motors.moveDegrees(2000, -2000, 125, BRAKE);
                motors.moveDegrees(4500, 4500, 130, BRAKE);
                while (lc > 115) {
                    csleft.getRawRGBC(&lr, &lg, &lb, &lc);
                    walltrackright(130, 20, 2000, 120);
                }
                leftMotor.brake();
                rightMotor.brake();

                motors.moveDegrees(2000, 2000, 40, BRAKE);
                motors.moveDegrees(-2000, 2000, 250, BRAKE);
                delay(500);
                motors.brake();
            } else {
                // turn left
                motors.moveDegrees(-4700, -4700, 120, BRAKE);
                motors.moveDegrees(-2000, 2000, 125, BRAKE);
                motors.moveDegrees(4500, 4500, 130, BRAKE);
                while (rc > 115) {
                    csright.getRawRGBC(&rr, &rg, &rb, &rc);
                    walltrackleft(130, 20, 2000, 120);
                }
                leftMotor.brake();
                rightMotor.brake();

                motors.moveDegrees(2000, 2000, 40, BRAKE);
                motors.moveDegrees(2000, -2000, 250, BRAKE);
                delay(500);
                motors.brake();
            }

            unsigned long startTime = millis();
            while (millis() - startTime < 800) {
                csleft.getRawRGBC(&lr, &lg, &lb, &lc);
                csright.getRawRGBC(&rr, &rg, &rb, &rc);
                linetrack(15, 30, 12, 3000, lc, rc);
            }
            motors.brake();
            motors.moveDegrees(-2000, -2000, 200, BRAKE);
            evo.playTone(1000, 200);
            motors.moveDegrees(2500, -2500, 375, BRAKE);
            delay(1000);
            evo.playTone(1000, 200);
            findline();
            motors.brake();
            delay(1000);
            evo.playTone(1000, 200);
            frontDistance = frontds.getDistance();
        } else {
            if (inLoopCount) {
                inLoopCount = false;
                criteriaCount = 0;
            }
        }
        linetrack(28, 32, 12, 3000, lc, rc);
    }
    motors.brake();
}

void loop() {
    delay(10);
    if (button.getButton(2) == 1) {
        esp_restart();
    }
}
