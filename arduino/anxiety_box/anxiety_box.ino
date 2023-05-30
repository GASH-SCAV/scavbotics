
#include <Wire.h>
#include <vl53l4cd_class.h>

#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Connect a stepper motor with 516 steps per revolution (1.8 degree)
// to motor port #2 (M3 and M4)
const int stepsPerRev = 516;
const int stepsPerHalfRev = 258;
const int motorRpm = 20;
Adafruit_DCMotor *motor_1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor_2 = AFMS.getMotor(2);

// ToF sensor
VL53L4CD sensor_vl53l4cd_sat(&Wire, A1);

// Global Defines
#define CFG_DO_TOF true
#define BUTTON_1_PIN A2
#define BUTTON_2_PIN A5

// Threshold for how close you need to be to turn it on
const int distanceThresholdSamples = 15;
const int distanceMove = 150;


void setup() {
  // Initialize serial for output.
  Serial.begin(115200);
  Serial.println("Starting...");

  // Initialize I2C
  Wire.begin();

  // Initialize the button
  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);

  /******
   * Setup for the ToF sensor
   ******/

  // Configure VL53L4CD satellite component.
  sensor_vl53l4cd_sat.begin();

  // Switch off VL53L4CD satellite component.
  sensor_vl53l4cd_sat.VL53L4CD_Off();

  //Initialize VL53L4CD satellite component.
  sensor_vl53l4cd_sat.InitSensor();

  // Program the highest possible TimingBudget, without enabling the
  // low power mode. This should give the best accuracy
  sensor_vl53l4cd_sat.VL53L4CD_SetRangeTiming(200, 0);

  // Start Measurements
  sensor_vl53l4cd_sat.VL53L4CD_StartRanging();

  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");

  motor_1->setSpeed(255);
  motor_2->setSpeed(200);
}


bool doBuzz = false;

void loop() {
#if false
  if (do_buttons) {
    int button_2_state = !digitalRead(BUTTON_2_PIN);
    int button_1_state = digitalRead(BUTTON_1_PIN);
    
    if (button_1_state) {
      Serial.println("Button 1");
      motor_1->step(1, FORWARD, DOUBLE);
      return;
    }
    if (button_2_state) {
      Serial.println("Button 2");
      motor_2->step(1, BACKWARD, DOUBLE);
      return;
    }
  }
#endif
  
  
  uint8_t NewDataReady = 0;
  VL53L4CD_Result_t results;
  char report[128];

  bool doMove = false;

  if (CFG_DO_TOF) {
    uint8_t status = sensor_vl53l4cd_sat.VL53L4CD_CheckForDataReady(&NewDataReady);
  
    if (!status && NewDataReady) {
      // (Mandatory) Clear HW interrupt to restart measurements
      sensor_vl53l4cd_sat.VL53L4CD_ClearInterrupt();
  
      // Read measured distance. RangeStatus = 0 means valid data
      sensor_vl53l4cd_sat.VL53L4CD_GetResult(&results);
      snprintf(report, sizeof(report), "Status = %3u, Distance = %5u mm, Signal = %6u kcps/spad, Sigma = %6u\r\n",
               results.range_status,
               results.distance_mm,
               results.signal_per_spad_kcps,
               results.sigma_mm);
      Serial.print(report);
      if (results.signal_per_spad_kcps > distanceThresholdSamples) {
        doBuzz = true;
        if (results.distance_mm < distanceMove) {
          doMove = true;
        }
      } else {
        doBuzz = false;
      }
    }
  }
  if (doBuzz) {
    motor_2->run(FORWARD);
    Serial.println("buzzing");
  } else {
    motor_2->run(RELEASE);
  }

  if (doMove) {

    Serial.println("Moving");
    sensor_vl53l4cd_sat.VL53L4CD_StopRanging();
    doBuzz = false;
    motor_2->run(RELEASE);
    
    // Start moving
    int choice = random(0, 10);
    Serial.println(choice);
    if (choice < 3) {
      motor_1->setSpeed(255);
      motor_1->run(FORWARD);
      delay(1000);
    } else if (choice < 6) {
      motor_1->setSpeed(150);
      motor_1->run(FORWARD);
      motor_2->run(FORWARD);
      delay(500);
      motor_1->run(RELEASE);
      motor_1->setSpeed(255);
      delay(500);
      motor_2->run(RELEASE);
      motor_1->run(FORWARD);
      delay(1000);
    } else if (choice < 9) {
      motor_1->setSpeed(150);
      motor_1->run(FORWARD);
      delay(2000);
    } else {
      motor_1->setSpeed(255);
      motor_1->run(FORWARD);
      delay(3000);
    }
    motor_1->run(RELEASE);

    Serial.println("Done");
    // Clear the depth sensor
    sensor_vl53l4cd_sat.VL53L4CD_ClearInterrupt();
    sensor_vl53l4cd_sat.VL53L4CD_GetResult(&results);
    sensor_vl53l4cd_sat.VL53L4CD_StartRanging();
  }
}
