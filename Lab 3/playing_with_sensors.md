# Lab #3: Playing with Sensors
## Date: September 11th, 2026

**Name:** Zachary Blue

In this lab, I will be reading a potentiometer's voltage and detecting touch with an ESP32. I am uploading code using VS Code and PlatformIO using a windows laptop. 

---

## Steps I Took:

- Opened the Lab 3 project in PlatformIO and created playing_with_sensors.md
- Renamed value.cpp to potentiometer.cpp and commented out other.cpp files
- Wired the potentiometer to the breadboard (GND to GND, VCC to 3V, OUT to A0)
- Wrote the Serial.print(analogRead(...)) line
- Debugged: added monitor_speed to platformio.ini and fixed the pin from 0 to A0 when readings were stuck
- Completed voltage.cpp, which included the sensorValue variable, float conversion formula, and changing delay to 200ms
- Recorded the potentiometer video
- Swapped in the touch sensor, wrote touch.cpp with the if/else + LED
- Recorded the touch sensor video, pushed everything

---

## Repository contents:

- **potentiometer.cpp** - reads the potentiometer's position as a raw analog value and prints that number straight to the serial monitor, without any conversion. 
- **voltage.cpp** - reads the potentiometer's analog value and converts it into the actual voltage using the ADC formula, printing the voltage every 200ms. 
- **touch.cpp** - reads the sensor on A0 and uses if/else logic: when the sensor detects a touch, it turns the onboard LED on and prints a message to the terminal. When contact is broken, it turns the LED off and prints a different message. Checks every 200ms. 

---

## Reflection


*This assignment took about an hour and a half to finish. The level of difficulty on this assignment was medium. The instructions were clear, but near the beginning of the lab, I had to do a bit of debugging due to a code error (which I originally didn't know if it was a hardware or software error). As of right now, I feel quite comfortable with the course content. Overall, I actually really liked this lab!*