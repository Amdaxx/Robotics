#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4LCD lcd;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4Buzzer buzzer;

unsigned int lineSensorValues[3];


void setup() {
  buttonA.waitForPress();

  lineSensors.initThreeSensors();
  proxSensors.initThreeSensors();
  buttonA.waitForRelease();

}


int MAX_NUMBER_HOUSES = 1; //Change number of houses depending on the scenarios 1 for scenario 1 more than 1 for scenario 2

int nooHouses = 0;

void loop() {
  if (nooHouses<MAX_NUMBER_HOUSES) {
    // Navigate the maze to find the house
    lineSensors.initThreeSensors();
    proxSensors.initThreeSensors();
    navigateMaze();
    if (detectHouse()) {
      // Assuming the delivery task takes some time
      delay(2000);
      // Turn around to start the return journey
      turnAround();
    }
  } else {
    navigateMaze();
  }
}

int t1 = 600; // Threshold for detecting a line (black space)
int t2 = 250; // Threshold for detecting white space

void navigateMaze() {
  unsigned long startTime = millis();

  while (millis() - startTime < 2000) { // Move straight for 2 seconds
    lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);

    if (lineSensorValues[0] > t1 || lineSensorValues[1] > t1 || lineSensorValues[2] > t1) { // Check if the middle sensor detects a black line
      motors.setSpeeds(0, 0); // Stop immediately if a black line is detected
      if(lineSensorValues[1]>t1) // If mid sensor detects a black line go backward
        goBackWard();
      break;
    }

    motors.setSpeeds(100, 100); // Move straight

    // Perform any necessary checks or adjustments
    delay(100);
  }

  motors.setSpeeds(0, 0); // Stop after 2 seconds or if a black line is detected
  delay(200);
  // Make a turning decision
  if (canTurnLeft() && canTurnRight()) {
    int number = random(1, 3); // generate a random number to take a right/left turn
    if(number == 1)
      turnLeft();
    else
      turnRight();
  }
  else if (canTurnLeft())
  {
    turnLeft();
  }
  else if (canTurnRight())
  {
    turnRight();
  }
   else {
    // If no valid turning option, turn around
    goBackWard();
    delay(500); // Back up a bit before turning around for better maneuvering
    turnAround();
  }
}

bool canTurnRight() {
  // reading right sensors to know if white space is avaliable
  return lineSensorValues[2] < t2;
}

bool canTurnLeft() {
  // reading left sensors to know if white space is avaliable
  return lineSensorValues[0] < t2;
}

void turnLeft() {
  motors.setSpeeds(-200, 200);
  delay(250);
  motors.setSpeeds(0, 0);
}

void turnRight() {
  motors.setSpeeds(200, -200);
  delay(250);
  motors.setSpeeds(0, 0);
}

void goBackWard() {
  motors.setSpeeds(-150, -150);
  delay(200);
  motors.setSpeeds(0, 0);
}

void turnAround() {
  motors.setSpeeds(100, -100);
  delay(500); 
  motors.setSpeeds(0, 0);
}

bool detectHouse() {
  uint16_t detectionThreshold = 5;  // Threshold for object detection
  proxSensors.read();     
  uint16_t proxLeft = proxSensors.countsFrontWithLeftLeds();
  uint16_t proxRight = proxSensors.countsFrontWithRightLeds();
  // Calculate the average of the left and right proximity sensor readings
  uint16_t proxAverage = (proxLeft + proxRight) / 2;

  if (proxAverage > detectionThreshold) {
    buzzer.playFrequency(1000, 500, 15); // Play a sound if an object is detected
    motors.setSpeeds(0, 0); // Stop the motors
    nooHouses++; // number of house detected increase by 1
    return true; 
  } else {
    return false;
  }
}