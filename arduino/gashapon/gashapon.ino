
#define USE_TFT_DISPLAY 1
#define USE_PROX_SENSOR 1
#define USE_LEDS 1


#if USE_TFT_DISPLAY
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#endif // USE_TFT_DISPLAY


#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 5
#define POWER_PIN    10


// create a neopixel strip
static const int num_pixels_total = 133;
static const int start_pixel = 13;
static const int num_pixels = num_pixels_total - start_pixel;
uint32_t color_array[num_pixels];
Adafruit_NeoPixel strip = Adafruit_NeoPixel(num_pixels_total, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void blank_pattern(uint32_t* color_array) {
  for (int i = 0; i < num_pixels; ++i) {
    color_array[i] = 0;
  }
}

void test_ruler(uint32_t* color_array) {
  for (int i = 0; i < num_pixels; ++i) {
    if (i % 10 == 0) {
      color_array[i] = strip.Color(255, 0, 0);
    }
    else {
      color_array[i] = 0;
    }
  }
}

void show_poland(bool polarity, int width) {
  int stripe_counter = 0;
  bool do_red = true;
  for (int i = 0; i < num_pixels; ++i) {
    if (stripe_counter > width) {
      do_red = !do_red;
      stripe_counter = 0;
    }
    stripe_counter++;

    if (do_red) {
      color_array[i] = strip.Color(255, 0, 0);
    } else {
      color_array[i] = strip.Color(255, 255, 255);
    }
  }
}

void show_pattern(uint32_t* color_array) {
  for (int i = 0; i < start_pixel; ++i) {
    strip.setPixelColor(i, 0);
  }
  for (int i = 0; i < num_pixels; ++i) {
    strip.setPixelColor(i+start_pixel, color_array[i]);
  }
  digitalWrite(POWER_PIN, HIGH);
  strip.show();
}

#if USE_PROX_SENSOR
// Proximity sensor
#include <Adafruit_VCNL4030.h>

Adafruit_VCNL4030 vcnl;
#endif // USE_PROX_SENSOR

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

#if USE_PROX_SENSOR
  if (!vcnl.begin()) {
    Serial.println(F("Could not find a valid VCNL4030 sensor, check wiring!"));
    while (1) {
      delay(10);
    }
  }
#endif // USE_PROX_SENSOR

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
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);

  display_test(0);
#endif // USE_TFT_DISPLAY


  // Neo-pixel setup
  // Set power pin to output
  pinMode(POWER_PIN, OUTPUT);
  // Disable the pin, we're not currently writing to the neopixels.
  digitalWrite(POWER_PIN, LOW);

  // This initializes the NeoPixel library.
  strip.begin();
}

int i = 0;
void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println(i++);
  // delay(100);
#if USE_TFT_DISPLAY
  // display_test(i);
#endif // USE_TFT_DISPLAY

#if USE_PROX_SENSOR
  uint16_t prox = vcnl.readProximity();
#else
  uint16_t prox = 50;
#endif
  // Serial.println(prox);
  if (prox > 1200) {
    Serial.print("Detected!");
    Serial.println(i++);
    display_test(1);
    delay(500);
    display_test(0);
  }

  // test_ruler(color_array);
  show_poland(true, 7);
  show_pattern(color_array);
  delay(1000);

}
