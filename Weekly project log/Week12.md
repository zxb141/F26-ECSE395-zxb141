# Week 12 Project Log
**Team:** Zachary Blue, Aedan Reagans, Lucas Benson

## Monday, November 9, 2026
**Individual:**
- Updated and uploaded the firmware through VS Code and PlatformIO using the Feather ESP32 V2 configuration.
- Learned to use the serial monitor at 115200 baud and compared its sensor readings with the device's observed behavior.
- Found that the green LED and buzzer pin assignments were swapped. Corrected the assignments to use GPIO 15 for the green LED and A5 for the buzzer, resolving the blinking green LED and missing sound.
- Repeatedly tested PIR detection with hand movements and crawling, and tested ultrasonic adult detection with standing approaches and walkaways.

**Group:**
- Prepared the validation testing procedure for three scenarios: a simulated child crawling toward and contacting the gate, the same scenario with a simulated adult present, and an adult walking past the device.
- Set up a taped approach path and a mounted iPhone to make the testing process more consistent.

## Wednesday, November 11, 2026
**Individual:**
- Investigated alarms that occurred shortly before adult detection and several seconds after adults walked away.
- Revised the sensor-decision logic so ultrasonic adult detection suppresses an alarm and clears an alarm that is already sounding.
- Experimented with PIR confirmation times and adult-detection hold times to balance responsiveness with false-alarm prevention.
- Investigated why the PIR output remained at `1` after movement stopped. Located its physical delay adjustment, turned it toward the minimum setting, and retested the output.

**Group:**
- Completed nine validation trials: three trials for each of the three planned scenarios.
- Recorded three test videos and collected serial sensor logs as testing evidence.
- Identified false adult alarms, inconsistent detection, alarms that lasted too long, and gaps between alarms.
- Collected initial feedback about alarm loudness and annoyance from a roommate and people in the lab.

## Friday, November 13, 2026
**Individual:**
- Investigated repeated alarms caused by one hand movement. Added a four-second quiet period before rearming, then evaluated its effect.
- Tested a three-second motion-confirmation period, but found that the PIR's approximately 1.5-second pulses prevented reliable alarm triggering.
- Revised the logic to accommodate gaps between PIR pulses, then later removed the PIR cooldown to improve responsiveness.
- Compared device behavior during USB debugging and after switching to battery power, including startup and warmup behavior.
- Helped draft the validation checkpoint's participant description, measures-collected, and evidence-of-testing sections.

**Group:**
- Created and distributed a 10-alarm preference survey through QR codes and messages to classmates and friends. Collected 39 responses from November 11 through November 13.
- Prepared the validation checkpoint documentation, including participant information, procedures, measures collected, testing evidence, lessons learned, and proposed improvements.
