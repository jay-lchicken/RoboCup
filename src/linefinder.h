#include "definitions.h"
#include "getcolor.h"

void findline(){
    csleft.getRawRGBC(&lr, &lg, &lb, &lc);
    csright.getRawRGBC(&rr, &rg, &rb, &rc);
    motors.move(0, 2000);
    leftMotor.resetAngle();
    rightMotor.resetAngle();
    const int motorFindAngle = 500;
    // int state = 0;
    while (rightMotor.getAngle() < motorFindAngle){
        if (rc< 150){
            motors.brake();
            return;
        }
        // csleft.getRawRGBC(&lr, &lg, &lb, &lc);
        csright.getRawRGBC(&rr, &rg, &rb, &rc);
    }
    
    motors.moveDegrees(0, -3000, motorFindAngle);
    motors.move(3000, 0);
    leftMotor.resetAngle();
    while (leftMotor.getAngle() < motorFindAngle){
        if (lc< 150){
            motors.brake();
            return;

        }
        csleft.getRawRGBC(&lr, &lg, &lb, &lc);
    }
    
    motors.brake();
    return;
}

bool findlinesilver(){
    csleft.getRawRGBC(&lr, &lg, &lb, &lc);
    csright.getRawRGBC(&rr, &rg, &rb, &rc);
    motors.move(0, 2000);
    leftMotor.resetAngle();
    rightMotor.resetAngle();
    const int motorFindAngle = 100;
    // int state = 0;
    while (rightMotor.getAngle() < motorFindAngle){
        csleft.getRawRGBC(&lr, &lg, &lb, &lc);
        csright.getRawRGBC(&rr, &rg, &rb, &rc);
        getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
        if (rcolor == 3){
            motors.brake();
            evo.playTone(1000, 500);
            evo.clearDisplay();
            evo.writeToDisplay("Black", 0, 0);
            evo.drawDisplay();
            return false;
        }
        csright.getRawRGBC(&rr, &rg, &rb, &rc);
    }
    motors.moveDegrees(0, -2000, motorFindAngle);
    motors.move(2000, 0);
    leftMotor.resetAngle();
    while (leftMotor.getAngle() < motorFindAngle){
        csleft.getRawRGBC(&lr, &lg, &lb, &lc);
        csright.getRawRGBC(&rr, &rg, &rb, &rc);
        getColor(lr, lg, lb, lc, rr, rg, rb, rc, &lcolor, &rcolor);
        if (lcolor == 3){
            motors.brake();
            evo.playTone(1000, 500);
            return false;

        }
        csleft.getRawRGBC(&lr, &lg, &lb, &lc);
    }
    motors.moveDegrees(-2000, 0, motorFindAngle);
    motors.brake();
    return true;
}