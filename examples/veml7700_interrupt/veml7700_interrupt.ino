/** @file veml7700_interrupt.ino
 *
 * @brief DevLab VEML7700 example for interrupt threshold configuration.
 *
 * @author UNIT Electronics
 *
 * @bug No known bugs.
 */

/******************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <Wire.h>
#include "DevLab_VEML7700.h"

/******************************************************************************
 * MACROS AND DEFINES
 *****************************************************************************/
#if defined(ARDUINO_ARCH_ESP32)
#define DEVLAB_VEML7700_SDA_PIN 6
#define DEVLAB_VEML7700_SCL_PIN 7
#endif

#define VEML7700_LOW_THRESHOLD 5
#define VEML7700_HIGH_THRESHOLD 100

/******************************************************************************
 * GLOBAL OBJECTS
 *****************************************************************************/
DevLab_VEML7700 veml = DevLab_VEML7700();

/******************************************************************************
 * PRIVATE FUNCTIONS
 *****************************************************************************/
static void initI2C(void) {
#if defined(ARDUINO_ARCH_ESP32)
  Wire.begin(DEVLAB_VEML7700_SDA_PIN, DEVLAB_VEML7700_SCL_PIN);
#else
  Wire.begin();
#endif
  Wire.setClock(400000);
}

static void printGain(uint8_t gain) {
  Serial.print(F("Gain: "));
  switch (gain) {
  case VEML7700_GAIN_1:
    Serial.println(F("1"));
    break;
  case VEML7700_GAIN_2:
    Serial.println(F("2"));
    break;
  case VEML7700_GAIN_1_4:
    Serial.println(F("1/4"));
    break;
  case VEML7700_GAIN_1_8:
    Serial.println(F("1/8"));
    break;
  default:
    Serial.println(F("Unknown"));
    break;
  }
}

static void printIntegrationTime(uint8_t integrationTime) {
  Serial.print(F("Integration Time (ms): "));
  switch (integrationTime) {
  case VEML7700_IT_25MS:
    Serial.println(F("25"));
    break;
  case VEML7700_IT_50MS:
    Serial.println(F("50"));
    break;
  case VEML7700_IT_100MS:
    Serial.println(F("100"));
    break;
  case VEML7700_IT_200MS:
    Serial.println(F("200"));
    break;
  case VEML7700_IT_400MS:
    Serial.println(F("400"));
    break;
  case VEML7700_IT_800MS:
    Serial.println(F("800"));
    break;
  default:
    Serial.println(F("Unknown"));
    break;
  }
}

/******************************************************************************
 * ARDUINO SETUP AND LOOP
 *****************************************************************************/
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("DevLab VEML7700 Interrupt Test"));
  initI2C();

  if (!veml.begin()) {
    Serial.println(F("Sensor not found"));
    while (1) {
      delay(10);
    }
  }

  Serial.println(F("Sensor found"));

  veml.setGain(VEML7700_GAIN_1_8);
  veml.setIntegrationTime(VEML7700_IT_100MS);
  veml.setLowThreshold(VEML7700_LOW_THRESHOLD);
  veml.setHighThreshold(VEML7700_HIGH_THRESHOLD);
  veml.interruptEnable(true);

  printGain(veml.getGain());
  printIntegrationTime(veml.getIntegrationTime());
}

void loop() {
  if (veml.enabled()) {
    Serial.print(F("raw ALS: "));
    Serial.println(veml.readALS());
    Serial.print(F("raw white: "));
    Serial.println(veml.readWhite());
    Serial.print(F("lux: "));
    Serial.println(veml.readLux());

    uint16_t irq = veml.interruptStatus();
    if (irq & VEML7700_INTERRUPT_LOW) {
      Serial.println(F("Low threshold"));
    }
    if (irq & VEML7700_INTERRUPT_HIGH) {
      Serial.println(F("High threshold"));
    }
  } else {
    Serial.println(F("Sensor is not enabled"));
  }

  delay(500);
}
