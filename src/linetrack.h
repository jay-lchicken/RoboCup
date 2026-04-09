#include "definitions.h"

void linetrack(int kp, int kd, int ks, int speed, int lc, int rc){   //10-100


    // Calculate error
    int leftValue = map(lc, linputmin, linputmax, outputMin, outputMax);
    int rightValue = map(rc, rinputmin, rinputmax, outputMin, outputMax);
    int error = leftValue - rightValue;

    // Calculate derivative
    static int lastError = 0;
    int derivative = error - lastError;
    lastError = error;
    // Calculate motor speeds
    int correction = kp * error + kd * derivative;
    int baseSpeed = speed - abs(error) * ks;
    int leftSpeed = baseSpeed + correction;
    int rightSpeed = baseSpeed - correction;

    // Set motor speeds
    leftMotor.run(int(leftSpeed));
    rightMotor.run(int(rightSpeed));


}