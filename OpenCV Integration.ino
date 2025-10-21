#include <ESP32Servo.h>

Servo baseServo;
Servo shoulderServo;
Servo elbowServo;
Servo gripServo;

// Define servo pins (adjust as needed)
int basePin = 27;
int shoulderPin = 25;
int elbowPin = 26;
int gripPin = 14;

void moveArmToRed();    // Forward declarations
void moveArmToGreen();
void moveArmToBlue();

void setup() {
  Serial.begin(115200);

  baseServo.setPeriodHertz(50);
  shoulderServo.setPeriodHertz(50);
  elbowServo.setPeriodHertz(50);
  gripServo.setPeriodHertz(50);

  baseServo.attach(basePin, 500, 2400);
  shoulderServo.attach(shoulderPin, 500, 2400);
  elbowServo.attach(elbowPin, 500, 2400);
  gripServo.attach(gripPin, 500, 2400);
}

void loop() {
  if (Serial.available() > 0) {
    char color = Serial.read();
    if (color == 'R') {
      moveArmToRed();
    } else if (color == 'G') {
      moveArmToGreen();
    } else if (color == 'B') {
      moveArmToBlue();
    }
  }
}

// Below are the motion functions

void moveArmToRed() {
  baseServo.write(90);
  shoulderServo.write(90);
  elbowServo.write(90);
  gripServo.write(180);
}

void moveArmToGreen() {
  baseServo.write(180);
  shoulderServo.write(45);
  elbowServo.write(135);
  gripServo.write(0);
}

void moveArmToBlue() {
  baseServo.write(0);
  shoulderServo.write(135);
  elbowServo.write(45);
  gripServo.write(0);
}
