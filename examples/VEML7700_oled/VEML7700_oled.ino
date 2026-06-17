/** @file VEML7700_oled.ino
 *
 * @brief DevLab VEML7700 example with SSD1306 OLED output.
 *
 * @author UNIT Electronics
 *
 * @bug No known bugs.
 */

/******************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "DevLab_VEML7700.h"

/******************************************************************************
 * MACROS AND DEFINES
 *****************************************************************************/
#define OLED_WIDTH 128
#define OLED_HEIGHT 32
#define OLED_I2C_ADDRESS 0x3C

#if defined(ARDUINO_ARCH_ESP32)
#define DEVLAB_VEML7700_SDA_PIN 6
#define DEVLAB_VEML7700_SCL_PIN 7
#endif

/******************************************************************************
 * GLOBAL OBJECTS
 *****************************************************************************/
DevLab_VEML7700 veml = DevLab_VEML7700();
Adafruit_SSD1306 display = Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT, &Wire);

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

/******************************************************************************
 * ARDUINO SETUP AND LOOP
 *****************************************************************************/
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("DevLab VEML7700 OLED Test"));
  initI2C();

  if (!veml.begin()) {
    Serial.println(F("Sensor not found"));
    while (1) {
      delay(10);
    }
  }
  Serial.println(F("Sensor found"));

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1) {
      delay(10);
    }
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.display();

  veml.setGain(VEML7700_GAIN_1);
  veml.setIntegrationTime(VEML7700_IT_100MS);
}

void loop() {
  float lux = veml.readLux();

  Serial.print(F("Lux: "));
  Serial.println(lux);

  display.clearDisplay();
  display.setCursor(0, 8);
  display.print(F("Lux "));
  display.println(lux);
  display.display();

  delay(250);
}
