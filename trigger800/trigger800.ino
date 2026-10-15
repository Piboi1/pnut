// Arduino + BMP390: raise TRIGGER_PIN when altitude above the pad reaches 800 ft.
// Power on at the pad (or at ambient pressure on the bench); the first second
// of readings sets the zero reference.
//
// Library: "Adafruit BMP3XX Library" (Library Manager).
// Wiring (I2C): VIN->3V3/5V per your breakout, GND->GND, SDA->SDA, SCL->SCL.
// Do NOT connect TRIGGER_PIN to the Pnut's DATA connector.
#include <Wire.h>
#include <Adafruit_BMP3XX.h>
#include "altitude_trigger.h"

const int   TRIGGER_PIN   = 7;      // goes HIGH at/above threshold
const float THRESHOLD_FT  = 800.0f;
const float HYSTERESIS_FT = 10.0f;
const int   ZERO_SAMPLES  = 50;

Adafruit_BMP3XX bmp;
alt::Trigger trigger(THRESHOLD_FT, HYSTERESIS_FT);
float padPressure = 0;  // Pa

void setup() {
  Serial.begin(115200);
  pinMode(TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIGGER_PIN, LOW);

  if (!bmp.begin_I2C()) {  // address 0x77; pass 0x76 if SDO is tied low
    Serial.println("BMP390 not found");
    while (true) delay(1000);
  }
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_2X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  bmp.performReading();  // discard first
  double sum = 0;
  for (int i = 0; i < ZERO_SAMPLES; i++) {
    while (!bmp.performReading()) {}
    sum += bmp.pressure;
    delay(20);
  }
  padPressure = sum / ZERO_SAMPLES;
  Serial.print("Pad pressure (Pa): ");
  Serial.println(padPressure);
}

void loop() {
  if (!bmp.performReading()) return;
  float altFt = alt::altitudeFt(bmp.pressure, padPressure);
  bool fired = trigger.update(altFt);
  digitalWrite(TRIGGER_PIN, fired ? HIGH : LOW);

  Serial.print(altFt, 1);
  Serial.print(" ft  ");
  Serial.println(fired ? "TRIGGERED" : "-");
  delay(20);
}
