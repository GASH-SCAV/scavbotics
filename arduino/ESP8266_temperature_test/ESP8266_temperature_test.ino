#include <OneWire.h>
#include <DallasTemperature.h>

// Variables for controlling temperature sensors
OneWire  g_oneWire(4);
DallasTemperature g_sensors(&g_oneWire);
static int g_nSensors = 0;

// Parameters controlling behavior
static const int g_resolution = 10;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  g_sensors.begin();
  g_nSensors = g_sensors.getDeviceCount();
  g_sensors.setResolution(g_resolution);

}

void loop() {
  Serial.print("Requesting temperatures...");
  g_sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.print("DONE\n");

  for (int i = 0; i < g_nSensors; ++i) {
    Serial.print("Device ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(g_sensors.getTempFByIndex(i));
    Serial.print("\n");
  }
  delay(1000);
}
