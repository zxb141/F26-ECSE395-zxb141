# Week 7 Project Log
**Team:** Zachary Blue, Aedan Reagans, Lucas Benson

## Monday, October 5th, 2026
**Individual:**
- Wrote sections D (Interface Definitions), E (Unknowns, Assumptions, and Risks), and G (Learning and Iteration Plan) of the System Architecture and Prototype Plan
- Worked through how the power chain works (9 V battery → regulator → 5 V sensors/ESP32 → 3.3 V logic) and why the ultrasonic ECHO and battery readings need voltage dividers
- Planned software logic according to the hardware connections, using mostly conditionals and states
- Reviewed Lectures 9–11 (analytical vs. empirical evaluation, experiment design) to frame the prototype tests with independent/dependent variables

**Group (Zoom):**
- Split up the System Architecture assignment by section; I took D, E, G
- Finalized the design direction: box clamped to the gate rails with slotted plates, PIR angled down to detect Ariana, ultrasonic angled up to detect adults walking past and suppress false alarms, vibration sensor as backup
- Decided on a simple on/off button on the box instead of a toggle switch
- Confirmed power plan: rechargeable 9 V with a regulator, Wi-Fi hardware included but notification software is a stretch goal