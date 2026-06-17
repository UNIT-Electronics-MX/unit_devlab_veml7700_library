/** @file veml7700_sweep.ino
 *
 * @brief DevLab VEML7700 example that sweeps gain, integration time and lux
 * modes.
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
  const char *label;
} gain_config_t;

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
static const gain_config_t gains[] = {
    {VEML7700_GAIN_1_4, "1/4"},
    {VEML7700_GAIN_1_8, "1/8"},
    {VEML7700_GAIN_1, "1"},
    {VEML7700_GAIN_2, "2"},
};

static const timing_config_t timings[] = {
    {VEML7700_IT_25MS, 25, "25"},
    {VEML7700_IT_50MS, 50, "50"},
    {VEML7700_IT_100MS, 100, "100"},
    {VEML7700_IT_200MS, 200, "200"},
    {VEML7700_IT_400MS, 400, "400"},
    {VEML7700_IT_800MS, 800, "800"},
};

static const lux_config_t luxModes[] = {
    {VEML_LUX_NORMAL, "NORMAL"},
    {VEML_LUX_AUTO, "AUTO"},
    {VEML_LUX_NORMAL_NOWAIT, "NORMAL_NOWAIT"},
    {VEML_LUX_CORRECTED, "CORRECTED"},
    {VEML_LUX_CORRECTED_NOWAIT, "CORRECTED_NOWAIT"},
};

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

static void scanI2CDevices(void) {
  Serial.println(F("Scanning I2C bus..."));
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print(F("Device found at 0x"));
      Serial.println(address, HEX);
    }
  }
}

static void printResults(luxMethod mode) {
  uint16_t raw = veml.readALS();
  uint16_t white = veml.readWhite();
  float lux = veml.readLux(mode);

  Serial.print(millis());
  Serial.print(F(";"));
  Serial.print(raw);
  Serial.print(F(";"));
  Serial.print(white);
  Serial.print(F(";"));
  Serial.println(lux);
}

static void sweepMeasurements(void) {
  for (uint8_t gainIndex = 0; gainIndex < 4; gainIndex++) {
    veml.setGain(gains[gainIndex].reg);
    for (uint8_t timingIndex = 0; timingIndex < 6; timingIndex++) {
      veml.setIntegrationTime(timings[timingIndex].reg);
      delay(timings[timingIndex].timeMs + 20);
      for (uint8_t luxIndex = 0; luxIndex < 5; luxIndex++) {
        Serial.print(gains[gainIndex].label);
        Serial.print(F(";"));
        Serial.print(timings[timingIndex].label);
        Serial.print(F(";"));
        Serial.print(luxModes[luxIndex].label);
        Serial.print(F(";"));
        printResults(luxModes[luxIndex].mode);
      }
    }
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

  Serial.println(F("DevLab VEML7700 Sweep Test"));
  initI2C();
  scanI2CDevices();

  if (!veml.begin()) {
    Serial.println(F("Sensor not found"));
    while (1) {
      delay(10);
    }
  }

  Serial.println(F("Sensor found"));
  Serial.println(F("gain;integration_ms;lux_mode;millis;raw;white;lux"));
}

void loop() {
  if (veml.enabled()) {
    sweepMeasurements();
  } else {
    Serial.println(F("Sensor is not enabled"));
    delay(500);
  }
}
