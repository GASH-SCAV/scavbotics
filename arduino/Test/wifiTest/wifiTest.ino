#include <ESP8266WiFi.h>

// Need to have secret.h in this directory with 
// #define WIFI_SSID "YOURSSID"
// #define WIFI_PASSWORD "yourpassword"
#include "secret.h"

static const char* ssid = WIFI_SSID;
static const char* password = WIFI_PASSWORD;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

  // Connect to the WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: " );
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: " );
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
}

const char* host = "wifitest.adafruit.com";

void loop() {
  // put your main code here, to run repeatedly:
  delay(5000);

  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
  }

  String url = "/testwifi/index.html";
  Serial.print("Requesting url: " );
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  delay(500);

  while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");

}
