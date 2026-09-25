#include <Arduino.h>


// zxb141: Function prototypes
float readDistance();
float middleOf(float a, float b, float c);


// zxb141: Define pins for ultrasonic sensor
const int TRIG_PIN = 27;
const int ECHO_PIN = 33; 

// zxb141: Define pins for LED traffic light
const int GREEN_LED_PIN = A5;
const int YELLOW_LED_PIN = 14; 
const int RED_LED_PIN = 32;

// zxb141: Define pin for buzzer
const int BUZZER_PIN = 15;


void setup() {
    
    // zxb141: Initialize serial communication at 115200 baud rate
    Serial.begin(115200);
    
    // zxb141: Set pin modes for ultrasonic sensor and LEDs
    pinMode(TRIG_PIN, OUTPUT); 
    pinMode(ECHO_PIN, INPUT); 

    // zxb141: Set pin modes for LED traffic light
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(YELLOW_LED_PIN, OUTPUT); 
    pinMode(RED_LED_PIN, OUTPUT);

     // zxb141: Set the buzzer pin as an output
    pinMode(BUZZER_PIN, OUTPUT);

}  



void loop() {
    
    // zxb141: Read distance from the ultrasonic sensor three times to get a more stable reading
    float d1 = readDistance();
    delay(10);
    float d2 = readDistance();
    delay(10);
    float d3 = readDistance();

    float distance = middleOf(d1, d2, d3); // zxb141: use the median to reject glitches

    // zxb141: Print the distance to the serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm away");

    // zxb141: Implement a glitch filter to ignore distances greater than 400 cm and less than 2 cm, as they are likely to be erroneous readings
    if (distance > 400 || distance < 2) {
        return;
    }

    // zxb141: If the distance is greater than 50 cm, turn on the green LED and turn off the yellow and red LEDs
    if (distance > 50) {
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(YELLOW_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, LOW);
        
        digitalWrite(BUZZER_PIN, LOW);
    }
    
    // zxb141: If the distance is between 20 cm and 50 cm, turn on the yellow LED and play a slow low-frequency tone on the buzzer
    else if (distance > 20) {
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(YELLOW_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, LOW);
        
        tone(BUZZER_PIN, 100);
        delay(300);
        noTone(BUZZER_PIN);
    }

    // zxb141: If the distance is less than or equal to 20 cm, turn on the red LED and play a fast high-frequency tone on the buzzer
    else {
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(YELLOW_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, HIGH);
        
        tone(BUZZER_PIN, 1000);
        delay(100);
        noTone(BUZZER_PIN);
    }

    delay(100); // zxb141: Wait for 100 milliseconds before the next measurement

}



// zxb141: Function to read distance from the ultrasonic sensor
float readDistance() {
    
    // zxb141: Send a 10-microsecond pulse to the trigger pin to initiate the ultrasonic measurement
    digitalWrite(TRIG_PIN, HIGH); 
    delayMicroseconds(10); 
    digitalWrite(TRIG_PIN, LOW); 

    // zxb141: Measure the duration of the echo pulse in microseconds
    long duration = pulseIn(ECHO_PIN, HIGH);

    // zxb141: Calculate the distance in centimeters using the speed of sound (343 m/s) and the duration of the echo pulse
    float distance = duration * 0.0343 / 2; // zxb141: Calculate the distance in centimeters
    
    return distance;

}



// zxb141: Return the median of three values to reject glitches
float middleOf(float a, float b, float c) {
    
    // zxb141: Check if 'a' is the middle value
    if ((a >= b && a <= c) || (a <= b && a >= c)) {
        return a;
    } 
    
    // zxb141: Check if 'b' is the middle value
    else if ((b >= a && b <= c) || (b <= a && b >= c)) {
        return b;
    }
    
    // zxb141: Check if 'c' is the middle value
    else {
        return c;
    }

}