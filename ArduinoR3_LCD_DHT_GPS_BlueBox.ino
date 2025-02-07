/*
  LiquidCrystal 16x2 LCD display

  The circuit:
 * 5V to Breadboard
 * Ground to Breadboard

 * LCD VSS pin to ground
 * LCD VDD pin to 5V
 * LCD V0 pin to potentiometer (center pin)
    - potentiometer (pin 1 = 5V, pin 3 = ground)

 * LCD RS pin to digital pin 7
 * LCD RW pin to ground
 * LCD E nable pin to digital pin 8

 * LCD D4 pin to digital pin 9
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 11
 * LCD D7 pin to digital pin 12

 * LCD A pin to potentiometer resistor to 5V
 * LCD K pin to ground

 * DHT Sensor (S on front left)
 * DHT Pin 1 to digital pin 2
 * DHT Pin 2 to 5V
 * DHT Pin 3 to gorund

 */

// include the libraries
#include <LiquidCrystal.h>
#include <PulseSensorPlayground.h>
#include "DHT.h"

#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

TinyGPSPlus gps;
SoftwareSerial ss(4, 3);

// Define the DHT Pin
#define DHTPIN 2

// Type of DHT Sensor
#define DHTTYPE DHT11

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;  // will store last time LED was updated

// constants won't change HH UL * MM UL * SS UL * MILS UL
const unsigned long interval = 15UL * 1000UL;  // Output every 15 seconds

int incrementalCounter = 0;
int humidLast = 0;
int tempLast = 0;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);


void setup() {
  Serial.begin(9600);
  ss.begin(9600);

  Serial.print("Simple TinyGPS library v. ");
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println("by Mikal Hart");
  Serial.println();

  // Setup LCD
  lcd.begin(16, 2);

  // Start DHT Sensor
  dht.begin();
}


void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {

    float humid = dht.readHumidity();
    int humidNow = truncf(humid * 10) / 10;

    if (humidNow != humidLast) {
      lcd.setCursor(0, 0);
      lcd.print("Humidity:       ");
      lcd.setCursor(10, 0);
      lcd.print(humidNow);
      lcd.print(" %");
      humidLast = humidNow;
    }

    float temp = dht.readTemperature();
    int tempNow = (temp * 1.8) + 32;

    if (tempNow != tempLast) {
      lcd.setCursor(0, 1);
      lcd.print("Temp    :       ");
      lcd.setCursor(10, 1);
      lcd.print(tempNow);
      lcd.print(" F");
      tempLast = tempNow;
    }

    Serial.print("Count: ");
    Serial.print(incrementalCounter);
    Serial.print(", Temp: ");
    Serial.print(tempNow);
    Serial.print("F, Humidity: ");
    Serial.print(humidNow);
    Serial.print("%.");
    Serial.println();

    while (ss.available() > 0) {
      if (gps.encode(ss.read())) {
        if (ss.available() > 0) {
          Serial.print("LAT=");
          Serial.print(gps.location.lat(), 6);
          Serial.print(", ");
          Serial.print("LONG=");
          Serial.print(gps.location.lng(), 6);
          Serial.print(", ");
          Serial.print("ALT=");
          Serial.print(gps.altitude.meters());
          Serial.println();

          if (incrementalCounter % 2 == 0) {
            lcd.setCursor(0, 0);
            lcd.print("Lat:            ");
            lcd.setCursor(5, 0);
            lcd.print(gps.location.lat());
            lcd.setCursor(0, 1);
            lcd.print("Lng:            ");
            lcd.setCursor(5, 1);
            lcd.print(gps.location.lng());
          }
        }
      }
    }

    incrementalCounter++;
    previousMillis += interval;
  }
}
