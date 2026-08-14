#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>
#include "MAX30100_PulseOximeter.h"
Servo servo1;
Servo servo2;

#define REPORTING_PERIOD_MS     1000

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte smile[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000
};
byte mod[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B11111,
  B00000,
  B00000,
  B00000
};
byte sad[] = {
  B00000,
  B00000,
  B01010,
  B00000,
  B01110,
  B10001,
  B00000,
  B00000
};

PulseOximeter pox;
uint32_t tsLastReport = 0;

void onBeatDetected()
{
  Serial.println("Beat!!!");
}

void setup()
{
  servo1.attach(9);
  servo2.attach(10);

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(1 , smile);
  lcd.createChar(2 , mod);
  lcd.createChar(3 , sad);
  lcd.setCursor(0, 0);
  lcd.print("      Pluse");
  lcd.setCursor(0, 1);
  lcd.print("    Oximeter");
   lcd.setCursor(0 , 0);
  delay(2000);
  lcd.clear();
   lcd.setCursor(7 , 0);
    lcd.print("BPM: ");   
    lcd.print(pox.getHeartRate());
    lcd.setCursor(7 , 1);
    lcd.print("Sp02: ");
    delay(2000);


 /* lcd.setCursor(1,1);
      lcd.write(3);//}
      lcd.write(3);//}
      delay(1500);

    lcd.clear();
     lcd.setCursor(1,1);
      lcd.write(1);//}
      lcd.write(16,1,1);//}
      delay(1500);
      
    lcd.clear();
    lcd.setCursor(1,1);
      lcd.write(2);//}
      lcd.write(               2);//}
      delay(1500);
      lcd.clear(); */

  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
    lcd.clear();
  } 
  else {
    Serial.println("SUCCESS");
    lcd.clear();
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{ 
    lcd.setCursor(0 , 0);
    lcd.print("BPM: ");   
    lcd.print(pox.getHeartRate());
    lcd.setCursor(0 , 1);
    lcd.print("Sp02: ");

  lcd.setCursor(4,0);
      lcd.write(3);//}
      delay(1500);

    lcd.clear();
     lcd.setCursor(4,0);
      lcd.write(1);//}
      delay(1500);
      
    lcd.clear();
  
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

    lcd.clear();
    lcd.setCursor(0 , 0);
    lcd.print("BPM: ");   
    lcd.print(pox.getHeartRate());
    lcd.setCursor(0 , 1);
    lcd.print("Sp02: ");
    lcd.print(pox.getSpO2());
    lcd.print("%");
    tsLastReport = millis();

    if (pox.getSpO2() >= 96) {
      lcd.setCursor(15 , 1);
      lcd.write(1); 
      servo1.write(100);
      delay(300);
      servo1.write(100);               
    }
    if (pox.getSpO2() <= 96){
      lcd.setCursor(4,0);
      lcd.print("mod");}

    if (pox.getHeartRate() <= 95 && pox.getSpO2() >= 91) {
      lcd.setCursor(15 , 1);
      lcd.write(2); 
      servo2.write(100);
      delay(300);
      servo2.write(100);                
    }
    // if (pox.getSpO2() <= 96){
      lcd.setCursor(4,0);
      lcd.write(2);//}
      delay(1500);
    lcd.clear();
  }
  
}
