#include <Arduino.h>
// This is required in PlatformIO but hidden in the standard Arduino IDE.

#define LED_PIN 13

void setup() {
  pinMode(LED_PIN, OUTPUT);
  // zxb141: defined the baud rates for the Serial connection
  Serial.begin(115200); // Start the Serial connection
}

void loop() {
  // zxb141: defined the text output for the Serial
  Serial.println("Hey Zach, its me! I'm your code printing out each of these letters to you!");
  
  // zxb141: create a heartbeat blinking pattern
  digitalWrite(LED_PIN, HIGH); // LED On
  delay(100); // Speed in milliseconds
  digitalWrite(LED_PIN, LOW); // LED Off
  delay(50); // Speed in milliseconds
  digitalWrite(LED_PIN, HIGH); // LED On
  delay(100); // Speed in milliseconds
  digitalWrite(LED_PIN, LOW); // LED Off
  delay(500); // Speed in milliseconds
}