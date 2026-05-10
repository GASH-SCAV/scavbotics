
#define USE_TFT_DISPLAY 1


#if USE_TFT_DISPLAY
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#endif // USE_TFT_DISPLAY


#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 5
#define POWER_PIN    10


// Proximity sensor
#include <Adafruit_VCNL4030.h>

Adafruit_VCNL4030 vcnl;


#if USE_TFT_DISPLAY
// Use dedicated hardware SPI pins for display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
#endif // USE_TFT_DISPLAY


#if USE_TFT_DISPLAY
void display_test(int i) {
  tft.fillScreen(ST77XX_BLACK);

  char buf[64];
  snprintf(buf, sizeof(buf), "Test: %d", i);
  tft.setTextSize(4);
  tft.setCursor(0, 0);
  tft.print(buf);
}
#endif // USE_TFT_DISPLAY


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("GASHapon machine starting up...");

  if (!vcnl.begin()) {
    Serial.println(F("Could not find a valid VCNL4030 sensor, check wiring!"));
    while (1) {
      delay(10);
    }
  }

  Serial.println(F("VCNL4030 Found!"));
  Serial.println();

// TFT Setup
#if USE_TFT_DISPLAY
  // For the Feather ESP32-S2, we need to enable I2C power first!
  // this section can be deleted for other boards
#if defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
  // turn on the I2C power by setting pin to opposite of 'rest state'
  pinMode(PIN_I2C_POWER, INPUT);
  delay(1);
  bool polarity = digitalRead(PIN_I2C_POWER);
  pinMode(PIN_I2C_POWER, OUTPUT);
  digitalWrite(PIN_I2C_POWER, !polarity);
#endif // defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)

  // Initialize display
  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize TFT
  tft.init(135, 240); // Init ST7789 240x135
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);

  display_test(0);
#endif // USE_TFT_DISPLAY
}

int i = 0;
void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println(i++);
  // delay(100);
#if USE_TFT_DISPLAY
  // display_test(i);
#endif // USE_TFT_DISPLAY

  uint16_t prox = vcnl.readProximity();
  Serial.println(prox);
  if (prox > 1200) {
    Serial.print("Detected!");
    Serial.println(i++);
    display_test(1);
    delay(500);
    display_test(0);
  }
}
