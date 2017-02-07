/*
 * Weather Station by IVEN1
 * 
   The LCD1602_I2C circuit :
   VCC  -  +5 V
   GND  -  Ground
   SDA  -  A4
   SCL  -  A5

   SD card SPI bus :
   MOSI - pin 11
   MISO - pin 12
   CLK - pin 13
   CS - pin 4 or 10

   RTC DS1307 :
   VCC  -  +5 V
   GND  -  Ground
   SDA  -  pin A4
   SCL  -  pin A5
*/

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include "RTClib.h"

LiquidCrystal_I2C lcd(0x3f, 16, 2);

#define CS    10
#define MOSI  11
#define MISO  12
#define CLK   13
#define wind A0
#define DHTPIN 2
#define DHTTYPE DHT22
#define OUTPUT_FILE  "Datalog.csv"

DHT dht(DHTPIN, DHTTYPE);
RTC_DS1307 rtc;
File wsFile;

unsigned long previousMillis = 0;
const long interval = 1000;
int windspeed = 0;

byte thermo[8] = {
  B00100,
  B01010,
  B01010,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110
};

byte water[8] = {
  B00100,
  B00100,
  B01010,
  B01010,
  B10001,
  B10001,
  B10001,
  B01110,
};

byte Cloud[8] = {
  B00000,
  B11011,
  B11011,
  B00100,
  B00100,
  B00100,
  B00100,
  B11111
};

void first_save() {
  wsFile = SD.open(OUTPUT_FILE, FILE_WRITE);
  if (wsFile) {
    wsFile.println("Weather Station by IVEN1");
    wsFile.print("Date");
    wsFile.print(",");
    wsFile.print("Time");
    wsFile.print(",");
    wsFile.print("Temperature");
    wsFile.print(",");
    wsFile.print("Humidity");
    wsFile.print(",");
    wsFile.println("Wind Speed");
    wsFile.close();
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Initial");
  SD.begin(CS);
  dht.begin();
  lcd.begin();

  lcd.backlight();
  lcd.createChar(1, thermo);
  lcd.createChar(2, water);
  lcd.createChar(3, Cloud);
  lcd.home();
  lcd.print(" Weather Station");
  lcd.setCursor(0, 1);
  lcd.print("     IVEN1      ");
  delay(1000);
  lcd.noBacklight();
  delay(1000);
  lcd.backlight();
  delay(1000);
  
  first_save();
  Serial.println("done...");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    int sensorValue = analogRead(A0);
    float outvoltage = sensorValue * (5.0 / 1023.0);
    windspeed = 6 * outvoltage;
    previousMillis = currentMillis;
  }

  int t = dht.readTemperature();
  int h = dht.readHumidity();

  //  Serial.print("wind: ");
  //  Serial.print(windspeed);
  //  Serial.print(" ");
  //  Serial.print("Humidity: ");
  //  Serial.print(h);
  //  Serial.print(" %\t");
  //  Serial.print("Temperature: ");
  //  Serial.print(t);
  //  Serial.println(" *C ");

  lcd.setCursor(0, 0);
  lcd.print(" ");
  lcd.write(1);
  lcd.print(":");
  lcd.print(t);
  lcd.print("C ");
  lcd.write(2);
  lcd.print(":");
  lcd.print(h);
  lcd.print("%RH   ");
  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.write(3);
  lcd.print(":Wind: ");
  lcd.print(windspeed);
  lcd.print(" m/s   ");
  delay(100);
}
