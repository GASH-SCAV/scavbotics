
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *motor_1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor_2 = AFMS.getMotor(2);




void setup() {
  // Initialize serial for output.
  Serial.begin(115200);
  Serial.println("Starting...");
  for (int i = 0; i < 10000; ++i) {
    Serial.println("looping a bit");
  }

  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");

  motor_1->setSpeed(255);
  motor_2->setSpeed(255);
}

void loop() {
  Serial.println("loop2");
  if (0) {
    motor_1->run(FORWARD);
    delay(10);
    motor_1->run(RELEASE);
    delay(100);
  }
  
  Serial.println("Forward");
  motor_2->run(FORWARD);
  delay(100);
  Serial.println("Release");
  motor_2->run(RELEASE);
  delay(1000);
  Serial.println("Backward");
  motor_2->run(BACKWARD);
  delay(100);
  Serial.println("Release");
  motor_2->run(RELEASE);
  delay(1000);
  

}
