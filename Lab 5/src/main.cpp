#include <Arduino.h>

// Adafruit Feather ESP32 V2
constexpr uint8_t trigPin = 27;
constexpr uint8_t echoPin = 33;
constexpr uint8_t greenLED = 15;
constexpr uint8_t yellowLED = 14;
constexpr uint8_t redLED = 32;
constexpr uint8_t buzzer = A5;
constexpr uint8_t pirPin = A4;
constexpr uint8_t vibrationPin = 13;

constexpr uint8_t PIR_ACTIVE_LEVEL = HIGH;
constexpr uint8_t VIBRATION_ACTIVE_LEVEL = HIGH;
constexpr uint8_t BUZZER_CHANNEL = 0;

constexpr uint32_t PIR_WARMUP_MS = 30000;
constexpr uint32_t PIR_CONFIRM_MS = 350;
constexpr uint32_t PIR_HOLD_MS = 5000;
constexpr uint32_t PING_MS = 100;
constexpr uint32_t ECHO_TIMEOUT_US = 12000;
constexpr uint32_t ADULT_HOLD_MS = 1500;
constexpr uint32_t VIBRATION_ALERT_MS = 2000;
constexpr uint32_t VIBRATION_QUIET_MS = 500;

uint32_t bootTime = 0;
float distanceCm = -1.0f;

volatile bool vibrationPending = false;
portMUX_TYPE vibrationMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onVibration() {
  portENTER_CRITICAL_ISR(&vibrationMux);
  vibrationPending = true;
  portEXIT_CRITICAL_ISR(&vibrationMux);
}

bool takeVibration() {
  portENTER_CRITICAL(&vibrationMux);
  const bool detected = vibrationPending;
  vibrationPending = false;
  portEXIT_CRITICAL(&vibrationMux);
  return detected;
}

bool updateAdultZone(uint32_t now) {
  static bool lastEcho = false;
  static bool previousEcho = false;
  static bool adultZone = false;
  static uint32_t lastConfirmed = 0;
  static uint32_t lastPing = 0;

  if (now - lastPing >= PING_MS) {
    lastPing = now;

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    const unsigned long echoUs =
        pulseIn(echoPin, HIGH, ECHO_TIMEOUT_US);

    distanceCm = echoUs * 0.01715f;
    if (distanceCm < 2.0f) distanceCm = -1.0f;

    // Two valid echoes in three pings confirm adult presence.
    const bool echoSeen = distanceCm > 0;

    if (echoSeen && (adultZone || lastEcho || previousEcho)) {
      adultZone = true;
      lastConfirmed = millis();
    }

    previousEcho = lastEcho;
    lastEcho = echoSeen;
  }

  if (adultZone && millis() - lastConfirmed >= ADULT_HOLD_MS) {
    adultZone = false;
  }

  return adultZone;
}

void updateOutputs(uint32_t now, bool ready, bool alarmOn) {
  static bool wasAlarmOn = false;
  static bool buzzerOn = false;
  static uint32_t alarmStarted = 0;

  if (alarmOn && !wasAlarmOn) {
    alarmStarted = now;
  }

  wasAlarmOn = alarmOn;

  const bool beep =
      alarmOn && ((now - alarmStarted) / 200) % 2 == 0;

  digitalWrite(greenLED, ready && !alarmOn);
  digitalWrite(redLED, beep);

  if (beep != buzzerOn) {
    ledcWrite(BUZZER_CHANNEL, beep ? 512 : 0);
    buzzerOn = beep;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(echoPin, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(vibrationPin, INPUT);

  for (const uint8_t pin :
       {trigPin, greenLED, yellowLED, redLED, buzzer}) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  // Initialize the buzzer's PWM once.
  ledcSetup(BUZZER_CHANNEL, 1800, 10);
  ledcAttachPin(buzzer, BUZZER_CHANNEL);
  ledcWrite(BUZZER_CHANNEL, 0);

  bootTime = millis();

  attachInterrupt(
      digitalPinToInterrupt(vibrationPin),
      onVibration,
      VIBRATION_ACTIVE_LEVEL == HIGH ? RISING : FALLING
  );

  Serial.println("Starting. PIR warmup takes 30 seconds.");
}

void loop() {
  static bool pirReady = false;
  static uint32_t pirHighSince = 0;
  static bool motionAlarm = false;
  static uint32_t lastMotion = 0;

  static bool vibrationAlarm = false;
  static bool vibrationArmed = true;
  static uint32_t vibrationStarted = 0;
  static uint32_t lastVibration = 0;

  static bool contactSinceLog = false;
  static uint32_t lastLog = 0;

  const bool adultZone = updateAdultZone(millis());
  const uint32_t now = millis();

  pirReady |= now - bootTime >= PIR_WARMUP_MS;

  const int pir = digitalRead(pirPin);
  const bool pirActive = pir == PIR_ACTIVE_LEVEL;

  if (!pirReady || !pirActive || adultZone) {
    pirHighSince = now;
  }

  if (!pirReady || adultZone) {
    motionAlarm = false;
  } else if (
      pirActive &&
      (motionAlarm || now - pirHighSince >= PIR_CONFIRM_MS)
  ) {
    motionAlarm = true;
    lastMotion = now;
  } else if (
      motionAlarm &&
      now - lastMotion >= PIR_HOLD_MS
  ) {
    motionAlarm = false;
  }

  const int vibration = digitalRead(vibrationPin);
  const bool vibrationPulse = takeVibration();

  const bool vibrationActive =
      vibrationPulse || vibration == VIBRATION_ACTIVE_LEVEL;

  contactSinceLog |= vibrationActive;

  if (vibrationAlarm &&
      now - vibrationStarted >= VIBRATION_ALERT_MS) {
    vibrationAlarm = false;
  }

  if (vibrationActive) {
    lastVibration = now;

    if (vibrationArmed && !adultZone) {
      vibrationAlarm = true;
      vibrationStarted = now;
    }

    vibrationArmed = false;
  }

  if (adultZone) {
    vibrationAlarm = false;
  }

  const bool alarmOn =
      !adultZone && (motionAlarm || vibrationAlarm);

  if (alarmOn) {
    // Prevent the buzzer's vibration from retriggering the sensor.
    vibrationArmed = false;
    lastVibration = now;
  } else if (
      !vibrationActive &&
      now - lastVibration >= VIBRATION_QUIET_MS
  ) {
    vibrationArmed = true;
  }

  updateOutputs(now, pirReady || adultZone, alarmOn);

  if (now - lastLog >= 500) {
    lastLog = now;

    const char* status = "GREEN";

    if (alarmOn) {
      status = vibrationAlarm
          ? "VIBRATION ALARM"
          : "MOTION ALARM";
    } else if (!pirReady && !adultZone) {
      status = "WARMING UP";
    }

    Serial.printf(
        "t=%lus | PIR=%s | Adult=%s | Vibration=%s | Distance=",
        static_cast<unsigned long>(now / 1000),
        pirActive ? "ON" : "OFF",
        adultZone ? "YES" : "NO",
        contactSinceLog ? "YES" : "NO"
    );

    if (distanceCm < 0) {
      Serial.print("no echo");
    } else {
      Serial.print(distanceCm, 1);
      Serial.print("cm");
    }

    Serial.print(" | State=");
    Serial.println(status);

    contactSinceLog = false;
  }

  delay(1);
}