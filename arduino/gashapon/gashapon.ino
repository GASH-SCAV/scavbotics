
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

static const uint8_t max_brightness = 50;


// create a neopixel strip
static const int num_pixels_total = 133;
static const int start_pixel = 13;
static const int num_pixels = num_pixels_total - start_pixel;
uint32_t color_array[num_pixels];
Adafruit_NeoPixel strip = Adafruit_NeoPixel(num_pixels_total, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);


// 
// PATTERN FUNCTIONS 
//

void blank_pattern(uint32_t* color_array) {
  for (int i = 0; i < num_pixels; ++i) {
    color_array[i] = 0;
  }
}

void test_ruler(uint32_t* color_array) {
  for (int i = 0; i < num_pixels; ++i) {
    if (i % 10 == 0) {
      color_array[i] = strip.Color(max_brightness, 0, 0);
    }
    else {
      color_array[i] = 0;
    }
  }
}

void show_poland(bool polarity, int width, int offset = 0) {
  for (int i = 0; i < num_pixels; ++i) {
    int in_pattern = (i + offset) % (width * 2);
    bool is_red = (in_pattern < width) ^ polarity;
    if (is_red) {
      color_array[i] = strip.Color(max_brightness, 0, 0);
    } else {
      color_array[i] = strip.Color(max_brightness, max_brightness, max_brightness);
    }
  }
}

void show_breathe(int time_in_breathe, int breathe_duration) {
  float fraction_in_duration = (float) time_in_breathe / (float) breathe_duration;
  if (fraction_in_duration >= 0.5) {
    fraction_in_duration = 1.0 - fraction_in_duration;
  }
  float brightness = 2.0 * fraction_in_duration;
  uint8_t brightness_8 = brightness * max_brightness;
  for (int i = 0; i < num_pixels; ++i) {
    color_array[i] = strip.Color(0, 0, brightness_8);
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

  blank_pattern(color_array);
}





/******************************
 * State functions begin here *
 ******************************/

using StateFn = void(int*, void**);

void decider_state(int* duration, void** next_state);
void start_state(int* duration, void** next_state);

// POLAND_FLASH
int _pattern_width = 0;
int _blink_duration = 0;
int _n_blinks = 0;
bool _poland_polarity = true;
void init_poland_flash(int pattern_width, int blink_duration, int n_blinks) {
  _pattern_width = pattern_width;
  _blink_duration = blink_duration;
  _n_blinks = n_blinks;
}
void state_poland_flash(int* duration, void** next_state) {
  Serial.println("Poland flash!");
  *duration = _blink_duration;
  show_poland(_poland_polarity, _pattern_width);
  _poland_polarity = !_poland_polarity;
  _n_blinks--;
  if (_n_blinks <= 0) {
    *next_state = (void*)decider_state;
  } else {
    *next_state = (void*)state_poland_flash;
  }
}

int _frame_duration = 0;
int _n_chases = 0;
int _offset = 0;
void init_poland_chase(int pattern_width, int frame_duration, int n_chases) {
  _pattern_width = pattern_width;
  _frame_duration = frame_duration;
  _n_chases = n_chases;
  _offset = 0;
}
void state_poland_chase(int* duration, void** next_state) {
  Serial.println("Poland chase!");
  *duration = _frame_duration;
  show_poland(true, _pattern_width, _offset);
  _offset++;
  _n_chases--;
  if (_n_chases <= 0) {
    *next_state = (void*)decider_state;
  } else {
    *next_state = (void*)state_poland_chase;
  }
}


int _toggle = 0;
unsigned long _party_duration = 0;
unsigned long _party_start_time = 0;
void init_poland_party(unsigned long party_duration) {
  _toggle = 0;
  _party_duration = party_duration;
  _party_start_time = millis();
}
void state_poland_party(int* duration, void** next_state) {
  Serial.println("Poland Party");
  *duration = 0;
  if (millis() - _party_start_time > _party_duration) {
    *next_state = (void*)decider_state;
  }
  if (_toggle == 0) {
    _toggle = 1;
    init_poland_flash(7, 750, 5);
    *next_state = (void*)state_poland_flash;
  } else {
    _toggle = 0;
    init_poland_chase(7, 100, 50);
    *next_state = (void*)state_poland_chase;
  }
}

unsigned int _breathe_duration = 0;
unsigned int _time_in_breathe = 0;
bool _breathe_up = true;
void init_breathe_state(int breathe_duration, int frame_duration) {
  _breathe_duration = breathe_duration;
  _frame_duration = frame_duration;
  _time_in_breathe = 0;
  _breathe_up = true;
}
void state_breathe(int* duration, void** next_state) {
  *duration = _frame_duration;

}

void decider_state(int* duration, void** next_state) {
  *duration = 0;
  Serial.println("Decider state");
  init_poland_party(1000 * 10);
  *next_state = (void*)state_poland_party;
}

void start_state(int* duration, void** next_state) {
  Serial.println("Start state");
  *duration = 0;
  *next_state = (void*)decider_state;
}

unsigned long last_led_time = 0;
int state_duration = 0;
StateFn* state = &start_state;


int i = 0;
void loop() {
  
  unsigned long this_time = millis();
  if (last_led_time == 0 || this_time - last_led_time > state_duration || this_time < last_led_time) {
    last_led_time = this_time;
    state(&state_duration, (void**)&state);
    show_pattern(color_array);
  }




}
