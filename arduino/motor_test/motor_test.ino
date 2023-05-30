
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *motor_1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor_2 = AFMS.getMotor(2);




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
  motor_2->setSpeed(100);
}

void loop() {
  Serial.println("Forward");
  motor_1->run(FORWARD);
  motor_2->run(FORWARD);
  delay(1000);
  Serial.println("Release");
  motor_1->run(RELEASE);
  motor_2->run(RELEASE);
  delay(500);
  Serial.println("Backward");
  motor_1->run(BACKWARD);
  motor_2->run(BACKWARD);
  delay(2000);

}
