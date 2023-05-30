#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <Adafruit_NeoPixel.h>


// Use dedicated hardware SPI pins for display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
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


#define METER_LEDS 56


void displaySensorDetails(void)
{
  sensor_t sensor;
  accel.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void displayDataRate(void)
{
  Serial.print  ("Data Rate:    "); 
  
  switch(accel.getDataRate())
  {
    case ADXL345_DATARATE_3200_HZ:
      Serial.print  ("3200 "); 
      break;
    case ADXL345_DATARATE_1600_HZ:
      Serial.print  ("1600 "); 
      break;
    case ADXL345_DATARATE_800_HZ:
      Serial.print  ("800 "); 
      break;
    case ADXL345_DATARATE_400_HZ:
      Serial.print  ("400 "); 
      break;
    case ADXL345_DATARATE_200_HZ:
      Serial.print  ("200 "); 
      break;
    case ADXL345_DATARATE_100_HZ:
      Serial.print  ("100 "); 
      break;
    case ADXL345_DATARATE_50_HZ:
      Serial.print  ("50 "); 
      break;
    case ADXL345_DATARATE_25_HZ:
      Serial.print  ("25 "); 
      break;
    case ADXL345_DATARATE_12_5_HZ:
      Serial.print  ("12.5 "); 
      break;
    case ADXL345_DATARATE_6_25HZ:
      Serial.print  ("6.25 "); 
      break;
    case ADXL345_DATARATE_3_13_HZ:
      Serial.print  ("3.13 "); 
      break;
    case ADXL345_DATARATE_1_56_HZ:
      Serial.print  ("1.56 "); 
      break;
    case ADXL345_DATARATE_0_78_HZ:
      Serial.print  ("0.78 "); 
      break;
    case ADXL345_DATARATE_0_39_HZ:
      Serial.print  ("0.39 "); 
      break;
    case ADXL345_DATARATE_0_20_HZ:
      Serial.print  ("0.20 "); 
      break;
    case ADXL345_DATARATE_0_10_HZ:
      Serial.print  ("0.10 "); 
      break;
    default:
      Serial.print  ("???? "); 
      break;
  }  
  Serial.println(" Hz");  
}

void displayRange(void)
{
  Serial.print  ("Range:         +/- "); 
  
  switch(accel.getRange())
  {
    case ADXL345_RANGE_16_G:
      Serial.print  ("16 "); 
      break;
    case ADXL345_RANGE_8_G:
      Serial.print  ("8 "); 
      break;
    case ADXL345_RANGE_4_G:
      Serial.print  ("4 "); 
      break;
    case ADXL345_RANGE_2_G:
      Serial.print  ("2 "); 
      break;
    default:
      Serial.print  ("?? "); 
      break;
  }  
  Serial.println(" g");  
}

void display_float(float f) {
  tft.fillScreen(ST77XX_BLACK);

  char buf[16];
  dtostrf(f, 10, 1, buf);
  tft.setTextSize(4);
  tft.setCursor(0, 0);
  tft.print(buf);
}

void setup(void) 
{
#ifndef ESP8266
  //while (!Serial); // for Leonardo/Micro/Zero
#endif
  Serial.begin(9600);
  Serial.println("Kabedonometer"); Serial.println("");
  
  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL345_RANGE_16_G);
  // accel.setRange(ADXL345_RANGE_8_G);
  // accel.setRange(ADXL345_RANGE_4_G);
  // accel.setRange(ADXL345_RANGE_2_G);
  
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  /* Display additional settings (outside the scope of sensor_t) */
  displayDataRate();
  displayRange();
  Serial.println("");

  accel.setDataRate(ADXL345_DATARATE_400_HZ);


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


  // Initialize the neopixels
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS (max = 255)
  initializeMeterColors();
  setMeter(METER_LEDS);
}

float x_sum = 0.0f, y_sum=0.0f, z_sum=0.0f;
float x_max = 0.0f, y_max=0.0f, z_max=0.0f;
float max_mag = 0.0f;
int event_count = 0;
float last_avg_x = 0.0f, last_avg_y = 0.0f, last_avg_z = 0.0f;
const int n_samples = 100;


uint32_t meter_colors[METER_LEDS];

void initializeMeterColors() {
  uint16_t start_hue = 65535/6;
  uint16_t end_hue = (65535/6)*5;
  uint16_t hue_delta = (65535/6)*2;
  uint16_t hue_step = hue_delta / METER_LEDS;
  Serial.print("Hue step:"); Serial.println(hue_step);
  for (int i = 0; i < METER_LEDS; ++i) {
    meter_colors[METER_LEDS-i-1] = strip.ColorHSV(start_hue + hue_step * i, 255, 255);
    //meter_colors[i] = strip.ColorHSV(start_hue, 255, 255);

  }
  
}

void setMeter(int meter_val) {
  if (meter_val < 0) {
    meter_val = 0;
  }
  
  const int meter_start = METER_LEDS-1;
  const int meter_end = meter_start-meter_val;

  for (int i = meter_start; i > meter_end; i--) {
    strip.setPixelColor(i, meter_colors[i]);
  }
  for (int i = meter_end; i >= 0; i--) {
    strip.setPixelColor(i, 0);
  }
}

int last_meter_val = 0;

// drop_step is the rate at which the lights "fall".
// It increases at 1 per second when new hits.
int drop_step = 1;

void loop(void) 
{
  /* Get a new sensor event */ 
  sensors_event_t event; 
  accel.getEvent(&event);
  event_count += 1;
  x_sum += event.acceleration.x;
  y_sum += event.acceleration.y;
  z_sum += event.acceleration.z;

  float res_x = event.acceleration.x - last_avg_x;
  float res_y = event.acceleration.y - last_avg_y;
  float res_z = event.acceleration.z - last_avg_z;

  max_mag = max(max_mag, res_x*res_x + res_y*res_y + res_z*res_z);

  if (event_count == n_samples) {
    last_avg_x = x_sum / n_samples;
    last_avg_y = y_sum / n_samples;
    last_avg_z = z_sum / n_samples;

    event_count = 0;
    display_float(max_mag);
    float difficulty = 200.0f;

    // Meter value should be between 0 and METER_LEDS
    int meter_val = min(sqrt(max_mag) / difficulty, 1.0f) * (float)METER_LEDS;
    if (meter_val >= last_meter_val) {
      last_meter_val = meter_val;
      drop_step = 1;
      setMeter(meter_val);
    } else {
      last_meter_val -= drop_step;
      drop_step += 2;
      setMeter(last_meter_val);
    }
    
    x_max = 0.0f;
    y_max = 0.0f;
    z_max = 0.0f;
    x_sum = 0.0f;
    y_sum = 0.0f;
    z_sum = 0.0f;
    max_mag = 0.0f;

    strip.show();

  }
}
