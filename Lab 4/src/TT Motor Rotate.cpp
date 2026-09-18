// zxb141: This code is for controlling a TT motor using an ESP32 microcontroller. It sets up the motor pins, configures them as outputs, and controls the motor's rotation in a sequence of clockwise, stop, counterclockwise, and stop again.

#include <Arduino.h>

// zxb141: define the motor pins for motor B
const int MOTOR_B_1A = A1; // zxb141: motor B pin 1A is connected to A1
const int MOTOR_B_1B = A0; // zxb141: motor B pin 1B is connected to A0

void setup() {
  // zxb141: Initialize Serial communication
  Serial.begin(115200);

  // zxb141: Set motor pins as OUTPUTs
  pinMode(MOTOR_B_1A, OUTPUT);
  pinMode(MOTOR_B_1B, OUTPUT); 

  Serial.println("Motor sequence will now begin!");
}

void loop() {
  // --- SECTION 1: Clockwise (5s) ---
  Serial.println("Spinning motor clockwise");
  // zxb141: Write HIGH to one pin and LOW to the other
  
  digitalWrite(MOTOR_B_1A, HIGH);
  digitalWrite(MOTOR_B_1B, LOW);
  
  delay(5000); // zxb141: motor B rotates clockwise for 5 seconds

  // --- SECTION 2: Stop (2s) ---
  Serial.println("Stopping motor now");
  
  // zxb141: Turn off the motor
  digitalWrite(MOTOR_B_1A, LOW);
  digitalWrite(MOTOR_B_1B, LOW);

  delay(2000); // zxb141: motor B stops for 2 seconds

  // --- SECTION 3: Counterclockwise (5s) ---
  Serial.println("Spinning motor counterclockwise");
  
  // zxb141: Write HIGH to one pin and LOW to the other
  digitalWrite(MOTOR_B_1A, LOW);
  digitalWrite(MOTOR_B_1B, HIGH);

  delay(5000); // zxb141: motor B rotates counterclockwise for 5 seconds

  // --- SECTION 4: Stop (2s) ---
  Serial.println("Stopping motor now");
  
  // zxb141: Turn off the motor
  digitalWrite(MOTOR_B_1A, LOW); 
  digitalWrite(MOTOR_B_1B, LOW);

  delay(2000); // zxb141: motor B stops for 2 seconds
}