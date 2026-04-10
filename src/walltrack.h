#include "definitions.h"

// Tracks right wall (rightds). Turns right to close gap, left to open it.
// Front sensor triggers a left turn for inner right-angle corners.
void walltrackleft(int kp, int kd, int speed = 2500, int distance = 150) {
    static int lastError = 0;
    int dist      = rightds.getDistance();
    int frontDist = frontds.getDistance();

    // Inner corner: front wall approaching, turn left to follow path
    if (frontDist > 0 && frontDist < 200) {
        motors.move(-1000, speed);
        lastError = 0;
        return;
    }

    // No reading (dist==0) or outer corner: wall lost, turn right to find it
    if (dist < 20 || dist > distance * 2) {
        motors.move(speed, 0);
        lastError = 0;
        return;
    }

    int error      = dist - distance;
    int derivative = error - lastError;
    lastError      = error;

    int correction = kp * error + kd * derivative;

    int left  = constrain(speed + correction, 0, 4095);
    int right = constrain(speed - correction, 0, 4095);
    motors.move(left, right);
}

// Tracks left wall (leftds). Turns left to close gap, right to open it.
// Front sensor triggers a right turn for inner right-angle corners.
void walltrackright(int kp, int kd, int speed = 2500, int distance = 150) {
    static int lastError = 0;
    int dist      = leftds.getDistance();
    int frontDist = frontds.getDistance();

    // Inner corner: front wall approaching, turn right to follow path
    if (frontDist > 0 && frontDist < 200) {
        motors.move(speed, -1000);
        lastError = 0;
        return;
    }

    // No reading (dist==0) or outer corner: wall lost, turn left to find it
    if (dist < 20 || dist > distance * 2) {
        motors.move(0, speed);
        lastError = 0;
        return;
    }

    int error      = dist - distance;
    int derivative = error - lastError;
    lastError      = error;

    int correction = kp * error + kd * derivative;

    int left  = constrain(speed - correction, 0, 4095);
    int right = constrain(speed + correction, 0, 4095);
    motors.move(left, right);
}

void silverisgaylikenate(){

}
