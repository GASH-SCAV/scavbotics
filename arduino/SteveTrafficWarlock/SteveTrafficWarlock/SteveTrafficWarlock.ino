#include <Adafruit_NeoPixel.h>

/*
   SteveTrafficWarlock.ico

   Firmware for light-up "traffic light staff" prop for Steve Clark: Traffic Warlock
*/

#define USE_TFT_DISPLAY 1

#include "Adafruit_LC709203F.h"

#if USE_TFT_DISPLAY
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#endif // USE_TFT_DISPLAY

#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 5
#define POWER_PIN    10




#if USE_TFT_DISPLAY
// Use dedicated hardware SPI pins for display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
#endif // USE_TFT_DISPLAY

Adafruit_LC709203F lc;

// create a neopixel strip
static const int n_rings = 12;
static const int ring_led_starts[] = {0, 17, 26, 35, 44, 53, 62, 71, 80, 89, 98, 107 };
static const int ring_led_ends[] = {17, 26, 35, 44, 53, 62, 71, 80, 89, 98, 107, 116 };
static const int num_pixels_total = 116;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(num_pixels_total, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void blank_rings(uint32_t* color_array) {
  for (int i = 0; i < num_pixels_total; ++i) {
    color_array[i] = 0;
  }
}

void set_ring(uint32_t* color_array, int ring_index, uint8_t r, uint8_t g, uint8_t b) {
  if (ring_index < 0 || ring_index >= n_rings) {
    return;
  }
  int ring_start = ring_led_starts[ring_index];
  int ring_end = ring_led_ends[ring_index];
  for (int i = ring_start; i < ring_end; ++i) {
    color_array[i] = strip.Color(r, g, b);
  }
}

void set_ring_red(uint32_t* color_array, int ring_index) {
  set_ring(color_array, ring_index, 255, 0, 0);
}

void set_ring_yellow(uint32_t* color_array, int ring_index) {
  set_ring(color_array, ring_index, 255, 200, 0);
}

void set_ring_green(uint32_t* color_array, int ring_index) {
  set_ring(color_array, ring_index, 0, 255, 0);
}

void show_rings(uint32_t* color_array) {
  for (int i = 0; i < num_pixels_total; ++i) {
    strip.setPixelColor(i, color_array[i]);
  }
  digitalWrite(POWER_PIN, HIGH);
  strip.show();
}


#if USE_TFT_DISPLAY
void display_battery(float voltage, float charge, float temp) {
  tft.fillScreen(ST77XX_BLACK);

  char buf[64];
  snprintf(buf, sizeof(buf), "%1.2fV\n%2.1f%%", voltage, charge);
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
  // Set power pin to output
  pinMode(POWER_PIN, OUTPUT);
  // Disable the pin, we're not currently writing to the neopixels.
  digitalWrite(POWER_PIN, LOW);

  // This initializes the NeoPixel library.
  strip.begin();
}


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

void set_all_red() {
  for (int i = 0; i < strip.numPixels(); ++i) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
  }
  //digitalWrite(POWER_PIN, HIGH);
  strip.show();
}

uint32_t color_array[num_pixels_total];

const int top_rings[] = {2, 3, 8, 9};
const int middle_rings[] = {1, 4, 7, 10};
const int bottom_rings[] = {0, 5, 6, 11};


/******************************
 * State functions begin here *
 ******************************/

using StateFn = void(int*, void**);

void decider_state(int* duration, void** next_state);

void red_even_green_odd(int* duration, void** next_state);
void red_even_yellow_odd(int* duration, void** next_state);
void green_even_red_odd(int* duration, void** next_state);
void yellow_even_red_odd(int* duration, void** next_state);

void red_even_green_odd(int* duration, void** next_state) {
  Serial.println("Red evens, Green odd");
  blank_rings(color_array);
  set_ring_red(color_array, top_rings[0]);
  set_ring_green(color_array, bottom_rings[1]);
  set_ring_red(color_array, top_rings[2]);
  set_ring_green(color_array, bottom_rings[3]);
  *duration=4000;
  *next_state = (void*)red_even_yellow_odd;
}

void red_even_yellow_odd(int* duration, void** next_state) {
  Serial.println("Red evens, Yellow odd");
  blank_rings(color_array);
  set_ring_red(color_array, top_rings[0]);
  set_ring_yellow(color_array, middle_rings[1]);
  set_ring_red(color_array, top_rings[2]);
  set_ring_yellow(color_array, middle_rings[3]);
  *duration=2000;
  *next_state = (void*)green_even_red_odd;
}

void green_even_red_odd(int* duration, void** next_state) {
  Serial.println("Green evens, Red odd");
  blank_rings(color_array);
  set_ring_green(color_array, bottom_rings[0]);
  set_ring_red(color_array, top_rings[1]);
  set_ring_green(color_array, bottom_rings[2]);
  set_ring_red(color_array, top_rings[3]);
  *duration=4000;
  *next_state = (void*)yellow_even_red_odd;
}

int regular_cycle_repeat_count = 0;
void yellow_even_red_odd(int* duration, void** next_state) {
  Serial.println("Yellow evens, Red odd");
  blank_rings(color_array);
  set_ring_yellow(color_array, middle_rings[0]);
  set_ring_red(color_array, top_rings[1]);
  set_ring_yellow(color_array, middle_rings[2]);
  set_ring_red(color_array, top_rings[3]);
  *duration=2000;
  regular_cycle_repeat_count--;
  if (regular_cycle_repeat_count) {
    *next_state = (void*)red_even_green_odd;
  } else {
    *next_state = (void*)decider_state;
  }
}

void start_regular_cycle(int* duration, void** next_state) {
  Serial.println("Start regular cycle");
  regular_cycle_repeat_count = 10;
  *duration = 0;
  *next_state = (void*)red_even_green_odd;
}

/// Blink cycle

int blink_repeat_count = 0;

void blink_red_off(int* duration, void** next_state);
void blink_red_on(int* duration, void** next_state);

void blink_red_off(int* duration, void** next_state) {
  Serial.println("Blink red off");
  blank_rings(color_array);
  *duration = 1500;
  *next_state = (void*)blink_red_on;
}
void blink_red_on(int* duration, void** next_state) {
  Serial.println("Blink red on");
  blank_rings(color_array);
  set_ring_red(color_array, top_rings[0]);
  set_ring_red(color_array, top_rings[1]);
  set_ring_red(color_array, top_rings[2]);
  set_ring_red(color_array, top_rings[3]);
  *duration = 1500;
  blink_repeat_count--;
  if (blink_repeat_count) {
    *next_state = (void*)blink_red_off;
  } else {
    *next_state = (void*)decider_state;
  }
}

void start_blinks_red(int* duration, void** next_state) {
  Serial.println("Start red blinks");
  blink_repeat_count = 10;
  *duration = 0;
  *next_state = (void*)blink_red_off;
}

/// Control states

int toggle = 0;
void decider_state(int* duration, void** next_state) {
  Serial.println("Decider state");
  *duration = 0;
  if (toggle == 0) {
    toggle = 1;
    *next_state = (void*)start_blinks_red;
  } else {
    toggle = 0;
    *next_state = (void*)start_regular_cycle;
  }
}

void start_state(int* duration, void** next_state) {
  Serial.println("Start state");
  *duration = 0;
  *next_state = (void*)decider_state;
}

unsigned long last_led_time = 0;
int state_duration = 0;
StateFn* state = &start_state;
void update_leds() {
  unsigned long this_time = millis();
  if (last_led_time == 0 || this_time - last_led_time > state_duration || this_time < last_led_time) {
    last_led_time = this_time;
    state(&state_duration, (void**)&state);
    show_rings(color_array);
  }
}
    

void loop() {
  do_battery_update();
  update_leds();
}
