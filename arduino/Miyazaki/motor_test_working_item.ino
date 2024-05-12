
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *motor_1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor_3 = AFMS.getMotor(3);
Adafruit_DCMotor *motor_4 = AFMS.getMotor(4);




void setup() {
  // Initialize serial for output.
  Serial.begin(115200);
  Serial.println("Starting...");

  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");

  motor_1->setSpeed(255);
  motor_3->setSpeed(255);
  motor_4->setSpeed(255);
}

void loop() {
  Serial.println("Run 1");
  for (int i = 0; i < 60; ++i) {
    motor_1->run(FORWARD);
    delay(20);
    motor_1->run(RELEASE);
    delay(150);
  }

  for (int j = 0; j < 5; ++j) {
    Serial.println("Run 3");
    motor_4->run(RELEASE);
    for (int i = 0; i < 17; ++i) {
  
        motor_3->run(FORWARD);
        delay(10);
        motor_3->run(RELEASE);
        delay(30);
      
    }
    delay(500);
    Serial.println("Run 4");
    motor_3->run(RELEASE);
    for (int i = 0; i < 17; ++i) {
  
        motor_4->run(FORWARD);
        delay(10);
        motor_4->run(RELEASE);
        delay(30);
      
    }
    delay(500);
  }

  

}
