// zxb141: in this code, we are going to use a touch sensor. if the touch sensor is activated, we will turn on the LED and print "I'm being touched!!" to the serial monitor. If the touch sensor is not activated, we will turn off the LED and print "I'm not being touched, I'm sad. Please touch me!" to the serial monitor.

#include <Arduino.h>

// zxb141: define the touch sensor pin as A0
const int sensorPin = A0;

// zxb141: define the LED pin as pin 13
#define LED_PIN 13

// zxb141: initialize the LED pin as an output and the serial monitor at 115200 baud rate
void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    // zxb141: store the analog value read from the touch sensor pin into sensorValue
    int sensorValue = analogRead(sensorPin);
    
    // zxb141: check if the sensor value is greater than 2000, if it is, turn on the LED and print "I'm being touched!!" to the serial monitor. If the sensor value is less than or equal to 2000, turn off the LED and print "I'm not being touched, I'm sad. Please touch me!" to the serial monitor.
    if (sensorValue > 2000) {
        digitalWrite(LED_PIN, HIGH); // zxb141: turn on the LED
        Serial.println("I'm being touched!!");
    } else {
        digitalWrite(LED_PIN, LOW); // zxb141: turn off the LED
        Serial.println("I'm not being touched, I'm sad. Please touch me!");
    }
    delay(200); // delay for 200 milliseconds
}