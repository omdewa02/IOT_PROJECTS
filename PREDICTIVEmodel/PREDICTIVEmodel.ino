#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
/*
#define REPORTING_PERIOD_MS     1000
#define LED_NORMAL 5
#define LED_ELEVATED 6
#define LED_RISK 7

PulseOximeter pox;
uint32_t tsLastReport = 0;

float prevBPM = 0;

void setup() {
    Serial.begin(9600);
    pinMode(LED_NORMAL, OUTPUT);
    pinMode(LED_ELEVATED, OUTPUT);
    pinMode(LED_RISK, OUTPUT);

    Serial.print("Initializing pulse oximeter..");

    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;); // halt
    } else {
        Serial.println("SUCCESS");
    }

    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
}

void classifyHeartRate(float currentBPM, float deltaBPM) {
    digitalWrite(LED_NORMAL, LOW);
    digitalWrite(LED_ELEVATED, LOW);
    digitalWrite(LED_RISK, LOW);

    if (currentBPM < 60) {
        Serial.println("Status: LOW Heart Rate");
        digitalWrite(LED_NORMAL, HIGH);
    } else if (currentBPM > 100) {
        if (deltaBPM > 20) {
            Serial.println("Status: RISKY! Sudden increase");
            digitalWrite(LED_RISK, HIGH);
        } else {
            Serial.println("Status: ELEVATED");
            digitalWrite(LED_ELEVATED, HIGH);
        }
    } else {
        if (deltaBPM > 15) {
            Serial.println("Status: ELEVATED (Change)");
            digitalWrite(LED_ELEVATED, HIGH);
        } else {
            Serial.println("Status: NORMAL");
            digitalWrite(LED_NORMAL, HIGH);
        }
    }
}

void loop() {
    pox.update();

    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        tsLastReport = millis();

        float currentBPM = pox.getHeartRate();
        float deltaBPM = abs(currentBPM - prevBPM);

        Serial.print("BPM: ");
        Serial.print(currentBPM);
        Serial.print(" | Δ: ");
        Serial.println(deltaBPM);

        classifyHeartRate(currentBPM, deltaBPM);
        prevBPM = currentBPM;
    }
}
*/










const int pulsePin = A0;
const int ledNormal = 5;
const int ledElevated = 6;
const int ledRisk = 7;

const int threshold = 550; // Tune this with Serial Plotter
const int sampleRate = 10; // ms
const int bpmWindow = 10;  // Store last 10 BPM values

unsigned long lastBeatTime = 0;
bool beatDetected = false;
int bpmHistory[bpmWindow];
int bpmIndex = 0;
bool bpmFilled = false;

void setup() {
  Serial.begin(9600);
  pinMode(pulsePin, INPUT);
  pinMode(ledNormal, OUTPUT);
  pinMode(ledElevated, OUTPUT);
  pinMode(ledRisk, OUTPUT);
  Serial.println("Starting heart rate monitor...");
}

void loop() {
  int signal = analogRead(pulsePin);
  static int lastSignal = 0;

  // Beat detection
  if (signal > threshold && !beatDetected) {
    unsigned long currentTime = millis();
    unsigned long delta = currentTime - lastBeatTime;

    if (delta > 300) { // 200ms = max 300 BPM filter
      int bpm = 60000 / delta;
      Serial.print("BPM: ");
      Serial.println(bpm);

      bpmHistory[bpmIndex++] = bpm;
      if (bpmIndex >= bpmWindow) {
        bpmIndex = 0;
        bpmFilled = true;
      }

      lastBeatTime = currentTime;
    }

    beatDetected = true;
  }

  if (signal < threshold) {
    beatDetected = false;
  }

  // Predict state based on BPM history
  if (bpmFilled) {
    float avg = computeAvg(bpmHistory, bpmWindow);
    float stddev = computeStdDev(bpmHistory, bpmWindow, avg);
    predictCondition(avg, stddev);
  }

  delay(sampleRate);
}

// Compute average of array
float computeAvg(int *arr, int size) {
  long sum = 0;
  for (int i = 0; i < size; i++) {
    sum += arr[i];
  }
  return (float)sum / size;
}

// Compute standard deviation
float computeStdDev(int *arr, int size, float mean) {
  float sum = 0;
  for (int i = 0; i < size; i++) {
    float diff = arr[i] - mean;
    sum += diff * diff;
  }
  return sqrt(sum / size);
}

// Simple predictive model
void predictCondition(float avgBPM, float stddev) {
  digitalWrite(ledNormal, LOW);
  digitalWrite(ledElevated, LOW);
  digitalWrite(ledRisk, LOW);

  if (avgBPM < 60) {
    Serial.println("Prediction: LOW Heart Rate");
    digitalWrite(ledNormal, HIGH);
  } else if (avgBPM > 100 && stddev > 8) {
    Serial.println("Prediction: RISKY (High & Irregular)");
    digitalWrite(ledRisk, HIGH);
  } else if (avgBPM > 100) {
    Serial.println("Prediction: ELEVATED");
    digitalWrite(ledElevated, HIGH);
  } else if (stddev > 10) {
    Serial.println("Prediction: IRREGULAR");
    digitalWrite(ledRisk, HIGH);
  } else if(avgBPM < 100 && avgBPM > 70){
    Serial.println("Prediction: NORMAL");
    digitalWrite(ledNormal, HIGH);
  }
}
