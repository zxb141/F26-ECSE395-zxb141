// BABY GATE ALARM: how to read this file
// The PIR senses movement through changes in body heat, not whether a baby is standing still
// The higher ultrasonic sensor assumes an adult is present when it receives enough echoes
// Adult detection cancels both alarms
// Without an adult detection, confirmed PIR motion or an armed vibration event can trigger sound
// true means yes, false means no, and ! means not
// && means both conditions must be true, while 
// 
//  means either condition can be true
#include <Arduino.h>

// SOUND SETTINGS: higher Hz means higher pitch.
// Aedan's pattern: 900 Hz, alternating equal periods of sound and silence.
// CHANGE MOTION PITCH HERE: higher Hz means a higher sound
// constexpr means this setting stays fixed while the device runs
constexpr uint16_t MOTION_TONE_HZ = 900;
// CHANGE VIBRATION PITCH HERE: this is separate from the motion pitch
constexpr uint16_t VIBRATION_TONE_HZ = 900;
// CHANGE BEEP SPEED HERE: shorter intervals produce faster alternating beeps
constexpr uint32_t MOTION_BEEP_MS = 200;    // 200 ms on, 200 ms off
constexpr uint32_t VIBRATION_BEEP_MS = 450; // 450 ms on, 450 ms off

// Breadboard pins
// The trigger wire tells the ultrasonic sensor to start a measurement
constexpr uint8_t trigPin = 27;
// The echo wire carries the pulse used to calculate distance
constexpr uint8_t echoPin = 33;
// These three pins control the traffic light module
// Green means ready with no alarm, red flashes during alarms, and yellow is unused
constexpr uint8_t greenLED = 15;
constexpr uint8_t yellowLED = 14;
constexpr uint8_t redLED = 32;
// The buzzer is physically connected to A5
constexpr uint8_t buzzer = A5;
// The PIR sends its motion signal to A4
constexpr uint8_t pirPin = A4;
// The vibration sensor sends its signal to pin 13
constexpr uint8_t vibrationPin = 13;
// This is an internal sound channel, not another physical pin
constexpr uint8_t BUZZER_CHANNEL = 0;
// HIGH means activity for these sensors
// Change these levels only if your actual sensors use the opposite signal
constexpr uint8_t PIR_ACTIVE_LEVEL = HIGH;
constexpr uint8_t VIBRATION_ACTIVE_LEVEL = HIGH;

// All times are milliseconds except ECHO_TIMEOUT_US
// TIMERS: 1000 milliseconds equals 1 second
// Warmup occurs at power-on or reset, not after each alarm
constexpr uint32_t PIR_WARMUP_MS = 30000;      // PIR startup settling time
// Require 0.35 seconds of uninterrupted PIR activity before starting a motion alarm
constexpr uint32_t PIR_CONFIRM_MS = 350;      // Ignore very brief PIR signals
// Keep alarming through PIR gaps for up to 5 seconds after its last active reading
// This also means sound can continue briefly after someone leaves
constexpr uint32_t PIR_HOLD_MS = 5000;        // Bridge gaps between PIR signals
constexpr uint32_t PING_MS = 100;             // Ultrasonic sampling interval
constexpr uint32_t ECHO_TIMEOUT_US = 12000;   // Limit waiting for an echo
// Keep assuming an adult for 1.5 seconds after the last confirming echo
constexpr uint32_t ADULT_HOLD_MS = 1500;      // Bridge missed adult echoes
// A vibration alarm lasts 2 seconds unless adult detection cancels it
constexpr uint32_t VIBRATION_ALERT_MS = 2000; // Length of a gate-tap alarm
// Require 0.5 seconds of quiet after alarms before accepting another vibration trigger
constexpr uint32_t VIBRATION_QUIET_MS = 500;  // Let buzzer vibration settle
// Print a status update every half second
constexpr uint32_t LOG_MS = 500;

// These names select which light and sound pattern to use
enum AlarmType { NO_ALARM, MOTION_ALARM, VIBRATION_ALARM };

// Current readings and decisions, shared by the functions below
// Remember the start time for PIR warmup
// uint32_t holds a nonnegative whole number, float holds decimals, and bool holds yes or no
uint32_t bootTime = 0;
// -1 means there was no usable distance measurement
float distanceCm = -1;
bool adultPresent = false;
bool pirReady = false;
bool pirActive = false;
bool motionAlarm = false;
bool vibrationAlarm = false;
// Remember short vibration events until they have appeared in the log
bool vibrationSinceLog = false;

// The interrupt remembers short vibration pulses between loop() calls
// volatile tells the compiler that an interrupt can change this flag
volatile bool vibrationPending = false;
// This lock protects the vibration flag when the main program and interrupt share it
portMUX_TYPE vibrationMux = portMUX_INITIALIZER_UNLOCKED;

// Runs automatically on a vibration pulse and remembers it for the main loop
// An interrupt briefly runs automatically when a vibration pulse arrives
// IRAM_ATTR places this small function in memory suitable for ESP32 interrupt handling
void IRAM_ATTR onVibration() {
  // Lock the shared flag before changing it inside the interrupt
  portENTER_CRITICAL_ISR(&vibrationMux);
  // Save the event even if the pulse ends before the next loop
  vibrationPending = true;
  // Release the lock so the main program can read the saved event
  portEXIT_CRITICAL_ISR(&vibrationMux);
}

// Returns true for a saved pulse or an active vibration signal, then clears the saved pulse
bool takeVibration() {
  // Protect the flag because the interrupt can change it at any time
  portENTER_CRITICAL(&vibrationMux);
  // Copy the saved event before clearing it
  const bool pulse = vibrationPending;
  // Consume this saved event so the same pulse is not counted again
  vibrationPending = false;
  portEXIT_CRITICAL(&vibrationMux);
  // Return yes if a pulse was saved OR the sensor is active right now
  return pulse || digitalRead(vibrationPin) == VIBRATION_ACTIVE_LEVEL;
}

// Checks the ultrasonic sensor for an adult and bridges brief missing echoes.
// Returns true while adult presence is assumed; this suppresses both alarms.
bool updateAdultZone(uint32_t now) {
  // static variables remember their values between function calls
  static uint32_t lastPing = 0;
  // Remember the latest echo that confirmed or maintained adult presence
  static uint32_t lastSeen = 0;
  // Remember two previous measurements so an isolated echo does not establish an adult
  static bool lastEcho = false;
  static bool previousEcho = false;
  static bool detected = false;

  // Subtract the previous time from the current time to measure elapsed time
  // Only start a new ultrasonic measurement every 100 milliseconds
  if (now - lastPing >= PING_MS) {
    lastPing = now;
    // Send a 10-microsecond trigger pulse to start the ultrasonic measurement
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Wait for the echo, with a timeout, then convert its travel time to centimeters
    // The conversion includes the sound traveling to the object and back
    distanceCm = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_US) * 0.01715f;
    // Discard timeouts and measurements closer than 2 centimeters
    if (distanceCm < 2) distanceCm = -1;

    // Two echoes in three pings establish adult presence.
    // Aim this sensor above the crawling path.
    const bool echo = distanceCm > 0;
    // Require this echo plus one of the last two echoes to establish an adult
    // Once adult presence is assumed, each usable echo refreshes the timer
    // Any usable echo counts here, so a wall or gate rail can also be detected
    if (echo && (detected || lastEcho || previousEcho)) {
      detected = true;
      lastSeen = millis();
    }
    // Move the measurement history along for the next ping
    previousEcho = lastEcho;
    lastEcho = echo;
  }
  // Release the adult assumption after 1.5 seconds without a confirming echo
  if (detected && millis() - lastSeen >= ADULT_HOLD_MS) detected = false;
  // Return the adult decision to the main loop
  return detected;
}

// Starts the motion alarm after a confirmed PIR signal, once warmup is complete.
// Holds it through short PIR gaps; detecting an adult cancels it immediately.
void updateMotion(uint32_t now) {
  // Remember when the PIR confirmation period started
  static uint32_t highSince = 0;
  // Remember the most recent active PIR reading during an alarm
  static uint32_t lastMotion = 0;

  // Restart confirmation during warmup, when PIR activity stops, or when an adult is detected
  if (!pirReady || !pirActive || adultPresent) highSince = now;
  // Warmup and adult detection both prevent a motion alarm
  if (!pirReady || adultPresent) {
    motionAlarm = false;
  // Start after confirmation, or keep an existing alarm active while the PIR signal is high
  } else if (pirActive && (motionAlarm || now - highSince >= PIR_CONFIRM_MS)) {
    motionAlarm = true;
    lastMotion = now; // Refresh the hold whenever the PIR detects motion
  // Stop once the last active PIR reading becomes 5 seconds old
  } else if (motionAlarm && now - lastMotion >= PIR_HOLD_MS) {
    motionAlarm = false;
  }
}

// Starts a brief alarm immediately for vibration when armed and no adult is detected.
// Waits for quiet after alarms so buzzer vibration does not repeatedly trigger it.
void updateVibration(uint32_t now) {
  // Armed means a new vibration event is allowed to start an alarm
  static bool armed = true;
  static uint32_t started = 0;
  static uint32_t lastVibration = 0;
  // Read current vibration activity and any saved interrupt pulse
  const bool active = takeVibration();
  // Keep the log flag true if vibration happened at any point since the previous print
  vibrationSinceLog |= active;

  // End the vibration alarm after its 2-second duration
  if (vibrationAlarm && now - started >= VIBRATION_ALERT_MS) vibrationAlarm = false;
  // Process vibration without waiting for PIR confirmation or PIR warmup
  if (active) {
    lastVibration = now;
    // Accept a new vibration alarm only when armed and no adult is assumed
    if (armed && !adultPresent) {
      vibrationAlarm = true; // A gate tap starts the alarm immediately
      started = now;
    }
    // Do not accept another vibration trigger yet
    armed = false;
  }
  // An adult detection cancels a vibration alarm already in progress too
  if (adultPresent) vibrationAlarm = false;

  // Prevent the buzzer from starting another vibration alarm
  // Keep vibration disarmed while either alarm is active and refresh its quiet timer
  if (motionAlarm || vibrationAlarm) {
    // Do not accept another vibration trigger yet
    armed = false;
    lastVibration = now;
  // Rearm only after alarms have stopped and vibration has stayed quiet long enough
  } else if (!active && now - lastVibration >= VIBRATION_QUIET_MS) {
    armed = true;
  }
}

// Controls the green light and blinks red in time with Aedan's buzzer pattern.
// Uses the sound settings at the top without delaying sensor checks for the beeps.
void updateOutputs(uint32_t now, bool ready, AlarmType alarm) {
  // Remember which pattern was playing so a new pattern starts from its first beep
  static AlarmType previousAlarm = NO_ALARM;
  static uint32_t started = 0;
  // Remember the current pitch to avoid unnecessary sound hardware updates
  static uint16_t playingHz = 0;

  // Restart the sound timing when the alarm type changes
  if (alarm != previousAlarm) {
    started = now;
    previousAlarm = alarm;
  }
  // Choose vibration settings for a gate tap and motion settings otherwise
  // The expression condition ? first : second selects first when true and second when false
  const bool contact = alarm == VIBRATION_ALARM;
  const uint16_t pitch = contact ? VIBRATION_TONE_HZ : MOTION_TONE_HZ;
  const uint32_t interval = contact ? VIBRATION_BEEP_MS : MOTION_BEEP_MS;

  // Alternate sound and silence without pausing sensor reads
  // Whole-number division counts how many sound or silence intervals have elapsed
  const uint32_t phase = (now - started) / interval;
  // % gives the remainder, so even phases play sound and odd phases are silent
  // No alarm always means silence
  const bool beep = alarm != NO_ALARM && phase % 2 == 0;
  // Show green only when ready with no alarm, including when an adult override is active
  digitalWrite(greenLED, ready && alarm == NO_ALARM);
  // Red is on exactly when the buzzer is sounding and off between beeps
  digitalWrite(redLED, beep);

  const uint16_t wantedHz = beep ? pitch : 0; // 0 means silence
  // Only send a new pitch or silence command when the requested output changes
  if (wantedHz != playingHz) {
    ledcWriteTone(BUZZER_CHANNEL, wantedHz);
    playingHz = wantedHz;
  }
}

// Prints labeled sensor readings and the current state to the Serial Monitor.
// Includes any vibration detected since the previous printed update.
void printStatus(uint32_t now, AlarmType alarm) {
  static uint32_t lastLog = 0;
  // Return means leave this function now if it is too soon to print again
  if (now - lastLog < LOG_MS) return;
  lastLog = now;
  Serial.printf("Vibration raw pin = %d\n", digitalRead(vibrationPin));
  Serial.printf("Vibration since last log = %s\n", vibrationSinceLog ? "YES" : "NO");

  // Translate the alarm number into a readable label
  const char* names[] = {"GREEN", "MOTION ALARM", "VIBRATION ALARM"};
  const char* status = names[alarm];
  // Explain the startup period when the normal green light is not yet on
  if (!pirReady && !adultPresent && alarm == NO_ALARM) status = "WARMING UP";

  // Print elapsed seconds and labeled readings
  // PIR is the raw sensor signal, while State includes the timers and adult override
  Serial.printf("t=%lus | PIR=%s | Adult=%s | Vibration=%s | Distance=",
      static_cast<unsigned long>(now / 1000), pirActive ? "ON" : "OFF",
      adultPresent ? "YES" : "NO", vibrationSinceLog ? "YES" : "NO");
  if (distanceCm < 0) {
    Serial.print("no echo");
  } else {
    // Display centimeters with one digit after the decimal point
    Serial.print(distanceCm, 1);
    Serial.print("cm");
  }
  Serial.print(" | State=");
  Serial.println(status);
  // Clear only the printed vibration history to start the next logging interval
  vibrationSinceLog = false;
}

// Runs once at power-on: configures pins, the buzzer, logging, and the vibration interrupt.
// Also starts the PIR warmup timer.
void setup() {
  // Use 115200 baud, matching the speed selected in the PlatformIO Serial Monitor
  Serial.begin(115200);
  // Configure sensor wires as inputs so the board reads their signals
  for (const uint8_t pin : {echoPin, pirPin, vibrationPin}) pinMode(pin, INPUT);
  // Configure controlled wires as outputs and start them off
  // Yellow stays off because no other function switches it on
  for (const uint8_t pin : {trigPin, greenLED, yellowLED, redLED, buzzer}) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
  // Initialize the ESP32 buzzer hardware once
  // LEDC is the ESP32 hardware that generates the buzzer wave
  // Configure it once, attach it to the buzzer pin, and start with sound off
  ledcSetup(BUZZER_CHANNEL, MOTION_TONE_HZ, 10);
  ledcAttachPin(buzzer, BUZZER_CHANNEL);
  ledcWrite(BUZZER_CHANNEL, 0);
  // millis() reports milliseconds since startup
  // Save this reading as the beginning of PIR warmup
  bootTime = millis();
  // Register the function that saves vibration pulses
  // RISING means LOW to HIGH and FALLING means HIGH to LOW
  attachInterrupt(digitalPinToInterrupt(vibrationPin), onVibration,
      VIBRATION_ACTIVE_LEVEL == HIGH ? RISING : FALLING);
  Serial.printf("Starting. PIR warmup takes %lu seconds.\n",
      static_cast<unsigned long>(PIR_WARMUP_MS / 1000));
}

// Repeats continuously: checks adults first, updates alarms, then updates lights and logs.
// Vibration takes priority over motion when choosing which alarm pattern to play.
void loop() {
  adultPresent = updateAdultZone(millis()); // Read the adult override first
  // Use one time reading for the remaining decisions in this loop pass
  const uint32_t now = millis();
  // Once warmup finishes, keep the PIR ready until the next power-on or reset
  pirReady = pirReady || now - bootTime >= PIR_WARMUP_MS;
  // Read the raw PIR signal before applying confirmation, hold, and adult suppression
  pirActive = digitalRead(pirPin) == PIR_ACTIVE_LEVEL;

  // Update motion first so vibration handling knows whether a motion alarm is active
  updateMotion(now);
  updateVibration(now);

  // Choose the output pattern, with vibration taking priority if both alarm flags are active
  AlarmType alarm = NO_ALARM;
  if (vibrationAlarm) alarm = VIBRATION_ALARM;
  else if (motionAlarm) alarm = MOTION_ALARM;
  // Apply the chosen alarm pattern to the lights and buzzer
  updateOutputs(now, pirReady || adultPresent, alarm);
  // Print the labeled status only when the logging timer is due
  printStatus(now, alarm);
  // Yield briefly for 1 millisecond, then check the sensors again
  delay(1);
}
