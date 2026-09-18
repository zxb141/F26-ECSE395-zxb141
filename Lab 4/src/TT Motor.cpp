// zxb141: This code is for controlling a TT motor using an ESP32 microcontroller. It sets up the motor pins, configures them as outputs, and controls the motor's rotation using PWM signals.

/* #include <Arduino.h>

// zxb141: define the motor pins for motor B
const int MOTOR_B_1A = A1; // zxb141: motor B pin 1A is connected to A1
const int MOTOR_B_1B = A0; // zxb141: motor B pin 1B is connected to A0


void setup() {

  pinMode(MOTOR_B_1A, OUTPUT);
  pinMode(MOTOR_B_1B, OUTPUT);  

  analogWrite(MOTOR_B_1A, 0); // zxb141: this pin is off at 0 PWM value
  analogWrite(MOTOR_B_1B, 200); // zxb141: motor B rotates clockwise at 200 PWM value

  delay(10000); // zxb141: motor B rotates clockwise for 10 seconds

  analogWrite(MOTOR_B_1A, 0);  // zxb141: motor B stops at 0 PWM value
  analogWrite(MOTOR_B_1B, 0); // zxb141: motor B stops at 0 PWM value

}

void loop() {

}

// Note:
// - Please modify the `analogWrite()`, swap the `analogWrite()`, and modify the `delay()`.
// - You don't have to put anything in the loop.
//      - If you would like to run the code again, please press the `RESET BUTTON` on your ESP32.

*/