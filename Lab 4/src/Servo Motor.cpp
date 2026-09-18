/*
// zxb141: This code is for controlling a servo motor using an ESP32 microcontroller. It sets up the servo pin, configures the pulse width range, and rotates the servo from 0 to 180 degrees and back in a loop.
#include <ESP32Servo.h>

// zxb141: initialize the servo object and define the pin for the servo
Servo myServo;
const int servoPin = A0;

// zxb141: define the minimum and maximum pulse widths for the servo
const int minPulseWidth = 500; // 0.5 ms
const int maxPulseWidth = 2500; // 2.5 ms

void setup() {
  // zxb141: Attach the servo to the specified pin and set its pulse width range
  myServo.attach(servoPin, minPulseWidth, maxPulseWidth);

  // zxb141: Set the PWM frequency for the servo
  myServo.setPeriodHertz(60); // Standard 50Hz servo
}

void loop() {
  // zxb141: Rotate the servo from 0 to 180 degrees
  for (int angle = 0; angle <= 180; angle++) {
    int pulseWidth;
    // zxb141: the map function is a function that maps 0 degrees to 500 pulse width, and 180 degrees to 2500 pulse width
    // zxb141: map(angle, min angle, max angle, min pulse width, max pulse width)
    pulseWidth = map(angle, 0, 180, minPulseWidth, maxPulseWidth);
    myServo.writeMicroseconds(pulseWidth);
    delay(50);
  }

  // zxb141: Rotate the servo from 180 to 0 degrees
  for (int angle = 180; angle >= 0; angle--) {
    int pulseWidth;
    // zxb141: the map function is a function that maps 0 degrees to 500 pulse width, and 180 degrees to 2500 pulse width
    // zxb141: map(angle, min angle, max angle, min pulse width, max pulse width)
    pulseWidth = map(angle, 0, 180, minPulseWidth, maxPulseWidth);
    myServo.writeMicroseconds(pulseWidth);
    delay(50);
  }
}

*/