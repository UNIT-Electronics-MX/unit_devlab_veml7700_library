/** @file veml7700.ino
 *
 * @brief DevLab VEML7700 example for configurable gain, integration time and
 * lux mode readings.
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

/******************************************************************************
 * TYPEDEFS
 *****************************************************************************/
typedef struct {
  uint8_t reg;
  uint16_t timeMs;
  const char *label;
} timing_config_t;

typedef struct {
  luxMethod mode;
  const char *label;
} lux_config_t;

/******************************************************************************
 * GLOBAL OBJECTS
 *****************************************************************************/
DevLab_VEML7700 veml = DevLab_VEML7700();

/******************************************************************************
 * CONSTANTS
 *****************************************************************************/
static const timing_config_t timings[] = {
    {VEML7700_IT_25MS, 25, "25 ms"},
    {VEML7700_IT_50MS, 50, "50 ms"},
    {VEML7700_IT_100MS, 100, "100 ms"},
    {VEML7700_IT_200MS, 200, "200 ms"},
    {VEML7700_IT_400MS, 400, "400 ms"},
    {VEML7700_IT_800MS, 800, "800 ms"},
};

static const lux_config_t luxModes[] = {
    {VEML_LUX_NORMAL, "NORMAL"},
    {VEML_LUX_AUTO, "AUTO"},
    {VEML_LUX_NORMAL_NOWAIT, "NORMAL_NOWAIT"},
    {VEML_LUX_CORRECTED, "CORRECTED"},
    {VEML_LUX_CORRECTED_NOWAIT, "CORRECTED_NOWAIT"},
};

static const uint8_t selectedTiming = 2;
static const uint8_t selectedLuxMode = 0;

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

static void configDevice(uint8_t gain, uint8_t integrationTime) {
  veml.setGain(gain);
  veml.setIntegrationTime(integrationTime);
}

static void printMeasurements(luxMethod mode) {
  uint16_t raw = veml.readALS();
  uint16_t white = veml.readWhite();
  float lux = veml.readLux(mode);

  Serial.print(F("Raw: "));
  Serial.print(raw);
  Serial.print(F(" | White: "));
  Serial.print(white);
  Serial.print(F(" | Lux: "));
  Serial.println(lux);
}

/******************************************************************************
 * ARDUINO SETUP AND LOOP
 *****************************************************************************/
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("DevLab VEML7700 Configurable Read"));
  initI2C();

  if (!veml.begin()) {
    Serial.println(F("Sensor not found"));
    while (1) {
      delay(10);
    }
  }

  Serial.println(F("Sensor found"));
  configDevice(VEML7700_GAIN_1_4, timings[selectedTiming].reg);

  Serial.print(F("Integration time: "));
  Serial.println(timings[selectedTiming].label);
  Serial.print(F("Lux mode: "));
  Serial.println(luxModes[selectedLuxMode].label);
}

void loop() {
  if (veml.enabled()) {
    printMeasurements(luxModes[selectedLuxMode].mode);
    delay(timings[selectedTiming].timeMs + 20);
  } else {
    Serial.println(F("Sensor is not enabled"));
    delay(500);
  }
}
