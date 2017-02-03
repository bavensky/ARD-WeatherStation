#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3f, 16, 2);

#define wind A0
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


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

void setup() {
  Serial.begin(9600);
  Serial.println("begin");
  dht.begin();
  lcd.begin();
  lcd.backlight();
  lcd.createChar(1, thermo);
  lcd.createChar(2, water);

  lcd.home();
  lcd.print(" Weather Station");
  lcd.setCursor(0, 1);
  lcd.print("     IVEN1      ");
  delay(1000);

  lcd.noBacklight();
  delay(1000);

  lcd.backlight();
}

void loop() {
  int sensorValue = analogRead(A0);
  float outvoltage = sensorValue * (5.0 / 1023.0);
  int Level = 6 * outvoltage;
  Serial.print("wind speed is ");
  Serial.print(Level);
  Serial.println(" level now");
  Serial.println();

  int t = dht.readTemperature();
  int h = dht.readHumidity();



  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");

  lcd.setCursor(0, 0);
  lcd.print("  ");
  lcd.write(1);
  lcd.print(":");
  lcd.print(t);
  lcd.print("C ");
  lcd.write(2);
  lcd.print(":");
  lcd.print(h);
  lcd.print("%RH   ");
  lcd.setCursor(0, 1);
  lcd.print("  Wind: ");
  lcd.print("m/s   ");
  delay(1000);
}
