#include <Time.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Servo.h>

Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo myservo4;

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


int sensorPin = A0; // select the input pin for the potentiometer
float sensorValue = analogRead(sensorPin); // variable to store the value coming from the sensor
int count=9;
int starttime = 0;
int heartrate = 0;
boolean counted = false;

void setup() {
// declare the ledPin as an OUTPUT:
lcd.init();
  lcd.backlight();
  pinMode(1,OUTPUT);
 // pinMode(11,INPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(7,OUTPUT);

  myservo1.attach(9);
  myservo2.attach(11);
  lcd.createChar(1 , smile);
  lcd.createChar(2 , mod);
  lcd.createChar(3 , sad);

/* digitalWrite(1, HIGH);
 digitalWrite(2, HIGH);
 digitalWrite(3, HIGH);
 digitalWrite(4, HIGH);
 digitalWrite(7, HIGH);
 delay(700);
 digitalWrite(1, LOW);
 digitalWrite(2, LOW);
 digitalWrite(3, LOW);
 digitalWrite(4, LOW);
 digitalWrite(7, LOW);
*/
   myservo1.write(170);
    delay(2000);
   myservo1.write(10);
   delay(2000);
  lcd.setCursor(2, 0);
  lcd.print("Pluse Monitor");
  delay(2000);
  lcd.clear();
pinMode(sensorPin, INPUT);
Serial.begin(9600);
}

void loop() {
// read the value from the sensor:
 
/*digitalWrite(12, HIGH);
   lcd.setCursor(3 , 0);
    lcd.print("BPM : ");
    lcd.print(sensorValue);
    lcd.setCursor(0, 1);
     lcd.write(2); 
    lcd.print(" ==NO VALUE== ");
    lcd.write(2);
    delay(500);
    digitalWrite(12,LOW);
    delay(1000);
    lcd.clear();
*/
lcd.clear();
 /*lcd.setCursor(0,0);
    lcd.print("Take long breathe");
    lcd.write(3);
    delay(1000);
    lcd.clear();
    delay(600);
    lcd.setCursor(0,0);
    lcd.print("take tablets by");
    delay(600);
    lcd.setCursor(2,1);
    lcd.print("DOCTOR DOOM");
    delay(4000);*/
myservo1.write(170);
    delay(2000);
   myservo1.write(10);
   delay(2000);

//sensorValue = analogRead(sensorPin);
if (70 < sensorValue <  550) // counting the pulse, and making sure I dont count
{ // each peak more than once

   digitalWrite(12, HIGH);
   lcd.setCursor(3 , 0);
    lcd.print("BPM : ");
    lcd.print(sensorValue);
    lcd.setCursor(1, 1);
     lcd.write(1); 
    lcd.print(" ==NORMAL== ");
    lcd.write(1);
    delay(500);
    digitalWrite(12,LOW);
    delay(1000);
    lcd.clear();
    
   
}

 if (sensorValue > 570)
{ 

 //if (sensorValue > 570){}

  lcd.setCursor(0,0);
    lcd.print("Take long breathe");
    lcd.write(3);
    delay(1000);
    lcd.clear();
    delay(600);
    lcd.setCursor(0,0);
    lcd.print("take tablets by");
    delay(600);
    lcd.setCursor(2,1);
    lcd.print("DOCTOR DOOM");
    delay(4000);

 lcd.clear();
    lcd.setCursor(2 , 0);
    lcd.print("BPM : ");
    lcd.print(sensorValue);
     lcd.write(3); 
     delay(1000);
}
}    

