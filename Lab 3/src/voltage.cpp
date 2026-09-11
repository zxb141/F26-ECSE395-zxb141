/* #include <Arduino.h>

// function prototype
float voltage(float analogvalue);

// Put your potentiometer pin assignment here
const int sensorPin = A0;

void setup() {
    Serial.begin(115200);
}

void loop() {
    // zxb141: store the analog value read from the potentiometer pin into sensorValue
    int sensorValue = analogRead(sensorPin);
    Serial.println(voltage(sensorValue));
    // zxb141: Printing the voltage value every 200 milliseconds
    delay(200); 
}

// function to calculate output voltage
float voltage(float analogvalue){
    float sensorVoltage;
    // zxb141: The formula converts the analog value to voltage based on a 12-bit ADC resolution (0-4095) and a reference voltage of 3.3V
    sensorVoltage = (analogvalue * 3.3) / 4095;
    return sensorVoltage;
}

*/