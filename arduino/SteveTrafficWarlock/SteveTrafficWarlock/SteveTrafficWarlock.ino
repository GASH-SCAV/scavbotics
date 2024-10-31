/*
 * SteveTrafficWarlock.ico
 * 
 * Firmware for light-up "traffic light staff" prop for Steve Clark: Traffic Warlock
 */

#define USE_TFT_DISPLAY 1
#define USE_NEOPIXELS 1

#include "Adafruit_LC709203F.h"

#if USE_TFT_DISPLAY
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#endif // USE_TFT_DISPLAY

#if USE_NEOPIXELS
#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 5
#define POWER_PIN    10

#define NUM_PIXELS 36
#endif  // USE_NEOPIXELS



#if USE_TFT_DISPLAY
// Use dedicated hardware SPI pins for display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
#endif // USE_TFT_DISPLAY

Adafruit_LC709203F lc;

#if USE_NEOPIXELS
// create a neopixel strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif  // USE_NEOPIXELS

#if USE_TFT_DISPLAY
void display_battery(float voltage, float charge, float temp) {
  tft.fillScreen(ST77XX_BLACK);

  char buf[64];
  snprintf(buf, sizeof(buf), "%1.2fV\n%2.1f%%", voltage, charge, temp); 
  tft.setTextSize(4);
  tft.setCursor(0, 0);
  tft.print(buf);
}
#endif // USE_TFT_DISPLAY


void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\nSteve Clark Traffic Warlock Cosplay");

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
#endif // USE_TFT_DISPLAY

  // Voltage meter setup
  if (!lc.begin()) {
    Serial.println(F("Couldnt find Adafruit LC709203F?\nMake sure a battery is plugged in!"));
    while (1) delay(10);
  }
  Serial.println(F("Found LC709203F"));
  Serial.print("Version: 0x"); Serial.println(lc.getICversion(), HEX);

  lc.setThermistorB(3950);
  Serial.print("Thermistor B = "); Serial.println(lc.getThermistorB());
  lc.setPackSize(LC709203F_APA_1000MAH);
  lc.setAlarmVoltage(3.8);

  // Display startup
#if USE_TFT_DISPLAY
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
#endif  // USE_TFT_DISPLAY


  // Neo-pixel setup
#if USE_NEOPIXELS
  // Set power pin to output
  pinMode(POWER_PIN, OUTPUT);
  // Disable the pin, we're not currently writing to the neopixels.
  digitalWrite(POWER_PIN, LOW);

  // This initializes the NeoPixel library.
  strip.begin();
#endif  // USE_NEOPIXELS
}

#if USE_NEOPIXELS
uint32_t Wheel(byte WheelPos) {
  // Input a value 0 to 255 to get a color value.
  // The colours are a transition r - g - b - back to r.
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbow(uint8_t wait) {
  Serial.println("Rainbow");
  
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    // turn on the Prop-Maker FeatherWing's power pin
    digitalWrite(POWER_PIN, HIGH);
    // write the pixel values
    strip.show();
    delay(wait);
  }
}

void set_all_red() {
  for (int i = 0; i < strip.numPixels(); ++i) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
  }
  digitalWrite(POWER_PIN, HIGH);
  strip.show();
}
#endif  // USE_NEOPIXELS

void do_battery_update() {
  static unsigned long last_time = 0;
  unsigned long this_time = millis();
  if (last_time == 0 || this_time - last_time > 2000 || this_time < last_time) {
    last_time = this_time;
    Serial.print("Batt_Voltage:");
    Serial.print(lc.cellVoltage(), 3);
    Serial.print("\t");
    Serial.print("Batt_Percent:");
    Serial.print(lc.cellPercent(), 1);
    Serial.print("\t");
    Serial.print("Batt_Temp:");
    Serial.println(lc.getCellTemperature(), 1);

#if USE_TFT_DISPLAY
    display_battery(lc.cellVoltage(), lc.cellPercent(), lc.getCellTemperature());
#endif  // USE_TFT_DISPLAY
  }
}


void loop() {
  do_battery_update();
#if USE_NEOPIXELS
  //set_all_red();
  //rainbow(20);
  set_all_red();
  delay(1000);
#endif  // USE_NEOPIXELS
}
