/*
   Weather Station by IVEN1

   Button LCD black light : digital pin 7

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

#define CS          4
#define MOSI        11
#define MISO        12
#define CLK         13
#define wind        A0
#define BLACKLIGHT  7
#define DHTPIN      2
#define DHTTYPE DHT22
#define OUTPUT_FILE  "Datalog.csv"

DHT dht(DHTPIN, DHTTYPE);
RTC_DS1307 rtc;
File wsFile;

unsigned long previous = 0;
unsigned long previousMillis = 0;
int windspeed = 0;
int count = 30, state = 0;

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

void datalog() {
  DateTime now = rtc.now();
  wsFile = SD.open(OUTPUT_FILE, FILE_WRITE);
  if (wsFile) {
    wsFile.print(now.day());
    wsFile.print("/");
    wsFile.print(now.month());
    wsFile.print("/");
    wsFile.print(now.year());
    wsFile.print(",");
    wsFile.print(now.hour());
    wsFile.print(":");
    wsFile.print(now.minute());
    wsFile.print(":");
    wsFile.print(now.second());
    wsFile.print(",");
    wsFile.print(dht.readTemperature());
    wsFile.print(",");
    wsFile.print(dht.readHumidity());
    wsFile.print(",");
    wsFile.println(windspeed);
    wsFile.close();
  }
}


void setup() {
  Serial.begin(9600);
  Serial.println("Initial");
  pinMode(BLACKLIGHT, INPUT_PULLUP);

  SD.begin(CS);
  dht.begin();

  rtc.begin();
  //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  lcd.begin();
  lcd.createChar(1, thermo);
  lcd.createChar(2, water);
  lcd.createChar(3, Cloud);

  lcd.home();
  lcd.print(" Weather Station");
  lcd.setCursor(0, 1);
  lcd.print("     IVEN1      ");
  lcd.backlight();
  delay(2000);
  lcd.noBacklight();
  delay(500);
  lcd.backlight();
  delay(500);
  lcd.noBacklight();

  first_save();
  Serial.println("done...");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    int sensorValue = analogRead(A0);
    float outvoltage = sensorValue * (5.0 / 1023.0);
    windspeed = 6 * outvoltage;
    previousMillis = currentMillis;
  }

  if (digitalRead(BLACKLIGHT) == 0) {
    lcd.backlight();
    state = 1;
  }

  if (state == 1) {
    unsigned long current = millis();
    if (current - previous >= 30000) {
      lcd.noBacklight();
      previous = current;
      state = 0;
    }
  }

  DateTime now = rtc.now();

  int t = dht.readTemperature();
  int h = dht.readHumidity();

  if (count == 60) count = 59;
  if (count > 60) count = count - 59;

  if (now.minute() == count) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Saving...      ");
    datalog();
    delay(2000);
    count += 30;
  }

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

  //  Serial.print("wind: ");
  //  Serial.print(windspeed);
  //  Serial.print(" ");
  //  Serial.print("Humidity: ");
  //  Serial.print(h);
  //  Serial.print(" %\t");
  //  Serial.print("Temperature: ");
  //  Serial.print(t);
  //  Serial.println(" *C ");
  //  Serial.print(now.year(), DEC);
  //  Serial.print('/');
  //  Serial.print(now.month(), DEC);
  //  Serial.print('/');
  //  Serial.print(now.day(), DEC);
  //  Serial.print(" ");
  //  Serial.print(now.hour(), DEC);
  //  Serial.print(':');
  //  Serial.print(now.minute(), DEC);
  //  Serial.print(':');
  //  Serial.print(now.second(), DEC);
  //  Serial.println();
}
