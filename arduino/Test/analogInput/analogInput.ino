#include <ESP8266WiFi.h>

#define BATTERY_IN A0



void setup() {
    Serial.begin(115200);

    Serial.println("Connecting...");
}

void loop() {
    int analog = analogRead(BATTERY_IN);
    Serial.print("Analog input: " );
    Serial.println(analog);
    delay(1000);
}
