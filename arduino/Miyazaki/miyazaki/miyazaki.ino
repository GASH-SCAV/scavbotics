
#include <Adafruit_MotorShield.h>
#include <Adafruit_NeoPixel.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *motor_1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor_2 = AFMS.getMotor(2);
Adafruit_DCMotor *motor_3 = AFMS.getMotor(3);
Adafruit_DCMotor *motor_4 = AFMS.getMotor(4);

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

#define LED_COUNT 60

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

void setup() {
  // Initialize serial for output.
  Serial.begin(115200);
  Serial.println("Starting...");

    while(true) {Serial.println("Test");}


  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");

  motor_1->setSpeed(255);
  motor_2->setSpeed(255);
  motor_3->setSpeed(40);
  motor_4->setSpeed(40);
  motor_3->run(FORWARD);
  motor_4->run(FORWARD);

  
  // Initialize the neopixels
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS (max = 255)
  initializeMeterColors();
  setMeter(30);
}

uint32_t meter_colors[LED_COUNT];

void initializeMeterColors() {
  uint16_t start_hue = 65535/6;
  uint16_t end_hue = (65535/6)*5;
  uint16_t hue_delta = (65535/6)*2;
  uint16_t hue_step = hue_delta / LED_COUNT;
  Serial.print("Hue step:"); Serial.println(hue_step);
  for (int i = 0; i < LED_COUNT; ++i) {
    meter_colors[LED_COUNT-i-1] = strip.ColorHSV(start_hue + hue_step * i, 255, 255);
    //meter_colors[i] = strip.ColorHSV(start_hue, 255, 255);

  }
  
}

void setMeter(int meter_val) {
  if (meter_val < 0) {
    meter_val = 0;
  }
  
  const int meter_start = LED_COUNT-1;
  const int meter_end = meter_start-meter_val;

  for (int i = meter_start; i > meter_end; i--) {
    strip.setPixelColor(i, meter_colors[i]);
  }
  for (int i = meter_end; i >= 0; i--) {
    strip.setPixelColor(i, 0);
  }
}

void loop() {
  if (0) {
    motor_1->run(FORWARD);
    delay(10);
    motor_1->run(RELEASE);
    delay(100);
  }
  
  Serial.println("Forward");
  motor_2->run(FORWARD);
  delay(250);
  Serial.println("Release");
  motor_2->run(RELEASE);
  delay(1000);
  Serial.println("Backward");
  motor_2->run(BACKWARD);
  delay(250);
  Serial.println("Release");
  motor_2->run(RELEASE);
  delay(1000);
  

}
