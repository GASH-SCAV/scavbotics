# scavbotics
Repository for all robotics and big-projects for GASH scav


# Arduino setup
## Installing Arduino IDE:

https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/using-arduino-ide

## Set up ESP8266 packages:

Enter http://arduino.esp8266.com/stable/package_esp8266com_index.json into Additional Board Manager URLs field in the Arduino v1.6.4+ preferences.

Next, use the Board manager to install the ESP8266 package.

## Board setup

When you've restarted, select Adafruit HUZZAH ESP8266 from the Tools->Board dropdown

80 MHz as the CPU frequency
115200 baud upload speed
**BE SURE TO SET SERIAL PORT** for programmer


## Simple tests

arduino/Test/blinkTest: blink the LED every 1s
arduino/Test/wifiTest: connect to wifi
