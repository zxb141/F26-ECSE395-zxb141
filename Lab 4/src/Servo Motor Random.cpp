#include <ESP32Servo.h>

// zxb141: Define the servo and the pin it is connected to, what is your servo pin?
Servo myServo;
const int servoPin = A0; // zxb141: define the pin for the servo motor

// zxb141: variable for random angle
int randomAngle;

// zxb141: Variable for pulse width
int pulseWidth;

// zxb141: Define the minimum and maximum pulse widths for the servo
const int minPulseWidth = 500; // 0.5 ms
const int maxPulseWidth = 2500; // 2.5 ms

void setup() {
  // zxb141: Attach the servo to the specified pin and set its pulse width range
  myServo.attach(servoPin, minPulseWidth, maxPulseWidth);

  // zxb141: Set the PWM frequency for the servo
  myServo.setPeriodHertz(50); // Standard 50Hz servo
}

void loop() {
    //  --- SECTION 1: Make a Random Angle Between 0 to 180 ---
    randomAngle = random(0, 181); // random(0, 181); returns a random value between 0 and 180

    // ---SECTION 2: Map Pulse Width with Angle
    pulseWidth = map(randomAngle, 0, 180, minPulseWidth, maxPulseWidth); // from Servo Motor.cpp, what did you learn from using map function?
    myServo.writeMicroseconds(pulseWidth); // writing pulse width to servo

    delay(random(500, 2000)); // delay for a random time between 500ms to 2000ms
}