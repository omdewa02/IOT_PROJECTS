
//#include <SPI.h>
#include <Wire.h>
/*#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 srituhobby = Adafruit_SSD1306(128, 64, &Wire);

#define sensor A0
#define Highpulse 540

int sX = 0;
int sY = 60;
int x = 0;
int Svalue;
int value;
long Stime = 0;
long Ltime = 0;
int count = 0;
int Bpm = 0;

void setup() {
  Serial.begin(9600);
  srituhobby.begin(SSD1306_SWITCHCAPVCC, 0x3C);// Address 0x3C for 128x32
  delay(1000);
  srituhobby.clearDisplay();
}

void loop() {
  Svalue = analogRead(sensor);
  Serial.println(Svalue);
  value = map(Svalue, 0, 1024, 0, 45);

  int y = 60 - value;

  if (x > 128) {
    x = 0;
    sX = 0;
    srituhobby.clearDisplay();
  }

  srituhobby.drawLine(sX, sY, x, y, WHITE);
  sX = x;
  sY = y;
  x ++;

  BPM();

  srituhobby.setCursor(0, 0);
  srituhobby.setTextSize(2);
  srituhobby.setTextColor(SSD1306_WHITE);
  srituhobby.print("BPM :");
  srituhobby.display();

}

void BPM() {

  if (Svalue > Highpulse) {
    Stime = millis() - Ltime;
    count++;

    if (Stime / 1000 >= 60) {
      Ltime = millis();
      Serial.println(count);
      srituhobby.setCursor(60, 0);
      srituhobby.setTextSize(2);
      srituhobby.setTextColor(SSD1306_WHITE);
      srituhobby.print(count);
      srituhobby.print("   ");
      srituhobby.display();
      count = 0;
    }
  }
}*/


#include <Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
int pulsePin = A0;                 // Pulse Sensor purple wire connected to analog pin A0
int blinkPin = 13;  

#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS     1000

Servo myservo1;
Servo myservo2;


#define LED 2
#define Buzzer 3
#define Sensor A1

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
  B10001,
  B01010,
  B01010,
  B00000,
  B00100,
  B01010,
  B10001,
  B00000
};
      

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                  
volatile int Signal;                
volatile int IBI = 600;              
volatile boolean Pulse = false;     
volatile boolean QS = false;       

static boolean serialVisual = true;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse 

volatile int rate[10];                      // array to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find IBI
volatile int P = 512;                      // used to find peak in pulse wave, seeded
volatile int T = 512;                     // used to find trough in pulse wave, seeded
volatile int thresh = 525;                // used to find instant moment of heart beat, seeded
volatile int amp = 100;                   // used to hold amplitude of pulse waveform, seeded
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = false;      // used to seed rate array so we startup with reasonable BPM
 
void setup()
{
 lcd.init();
 lcd.backlight();
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
                                    // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE, 
                                    // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN
                                    //   analogReference(EXTERNAL);   
 lcd.begin(16,2); 
  lcd.createChar(1 ,smile);
  lcd.createChar(2 ,mod);
  lcd.createChar(3 ,sad);
  myservo1.attach(6);
  myservo2.attach(9);
  Serial.begin(9600);
}


//  Where the Magic Happens
/*void loop()
{
   serialOutput();
   
  if (QS == true) // A Heartbeat Was Found
    {     
      // BPM and IBI have been Determined
      // Quantified Self "QS" true when arduino finds a heartbeat
      serialOutputWhenBeatHappens(); // A Beat Happened, Output that to serial.     
      QS = false; // reset the Quantified Self flag for next time    
    }
     
  delay(200); //  take a break
}
*/

void interruptSetup()
{     
  // Initializes Timer2 to throw an interrupt every 2mS.
  TCCR2A = 0x02;    
  TCCR2B = 0x06;     
  OCR2A = 0X7C;      
  TIMSK2 = 0x02;     
  sei();                
} 

void serialOutput()
{   
 if (serialVisual == true)
  {  
     arduinoSerialMonitorVisual('-', Signal);   
  } 
 else
  { 
      sendDataToSerial('S', Signal);    
   }        
}

void serialOutputWhenBeatHappens()
{    
 if (serialVisual == true) //  Code to Make the Serial Monitor Visualizer Work
   {   lcd.setCursor(0,0);         
     Serial.print(" Heart-Beat Found ");  
    /* Serial.print("BPM: ");
     Serial.print(BPM);
     lcd.clear();
      lcd.setCursor(0,0);
     lcd.write(1);
     lcd.print(" ==NORMAL== ");
     lcd.write(1);
     lcd.setCursor(4,1);
     lcd.print("BPM: ");
     lcd.setCursor(9,1);
     lcd.print(BPM);
     delay(300);
     
    lcd.clear();
     */
   }
 else
   {
     sendDataToSerial('B',BPM);   
     sendDataToSerial('Q',IBI);   
   }   
}

void arduinoSerialMonitorVisual(char symbol, int data )
{    
  const int sensorMin = 0;      
  const int sensorMax = 1024;   
  int sensorReading = data; 
  int range = map(sensorReading, sensorMin, sensorMax, 0, 11);
  
}


void sendDataToSerial(char symbol, int data )
{
   Serial.print(symbol);
   Serial.println(data);                
}

ISR(TIMER2_COMPA_vect) //triggered when Timer2 counts to 124
{  
  cli();                                      // disable interrupts while we do this
  Signal = analogRead(pulsePin);              // read the Pulse Sensor 
  sampleCounter += 2;                         // keep track of the time in mS with this variable
  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise
                                              //  find the peak and trough of the pulse wave
  if(Signal < thresh && N > (IBI/5)*3) // avoid dichrotic noise by waiting 3/5 of last IBI
    {      
      if (Signal < T) // T is the trough
      {                        
        T = Signal; // keep track of lowest point in pulse wave 
      }
    }

  if(Signal > thresh && Signal > P)
    {          // thresh condition helps avoid noise
      P = Signal;                             // P is the peak
    }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N > 250)
  {                                   // avoid high frequency noise
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) )
      {        
        Pulse = true;                               // set the Pulse flag when we think there is a pulse
        digitalWrite(blinkPin,HIGH);                // turn on pin 13 LED
        IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
        lastBeatTime = sampleCounter;               // keep track of time for next pulse
  
        if(secondBeat)
        {                        // if this is the second beat, if secondBeat == TRUE
          secondBeat = false;                  // clear secondBeat flag
          for(int i=0; i<=9; i++) // seed the running total to get a realisitic BPM at startup
          {             
            rate[i] = IBI;                      
          }
        }
  
        if(firstBeat) // if it's the first time we found a beat, if firstBeat == TRUE
        {                         
          firstBeat = false;                   // clear firstBeat flag
          secondBeat = true;                   // set the second beat flag
          sei();                               // enable interrupts again
          return;                              // IBI value is unreliable so discard it
        }   
      // keep a running total of the last 10 IBI values
      word runningTotal = 0;                  // clear the runningTotal variable    

      for(int i=0; i<=8; i++)
        {                // shift data in the rate array
          rate[i] = rate[i+1];                  // and drop the oldest IBI value 
          runningTotal += rate[i];              // add up the 9 oldest IBI values
        }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values 
      BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
      QS = true;                              // set Quantified Self flag 
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }                       
  }

  if (Signal < thresh && Pulse == true)
    {   // when the values are going down, the beat is over
      digitalWrite(blinkPin,LOW);            // turn off pin 13 LED
      Pulse = false;                         // reset the Pulse flag so we can do it again
      amp = P - T;                           // get amplitude of the pulse wave
      thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
      P = thresh;                            // reset these for next time
      T = thresh;
    }

  if (N > 2500)
    {                           // if 2.5 seconds go by without a beat
      thresh = 512;                          // set thresh default
      P = 512;                               // set P default
      T = 512;                               // set T default
      lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date        
      firstBeat = true;                      // set these to avoid noise
      secondBeat = false;                    // when we get the heartbeat back
    }

  sei();                                   // enable interrupts when youre done!
}
void loop()
{
   serialOutput();

   Serial.print("BPM: ");
     Serial.print(BPM);
     delay(100);
     lcd.clear();
      lcd.setCursor(1,0);
     lcd.write(1);
     lcd.print(" ==NORMAL== ");
     lcd.write(1);
     lcd.setCursor(4,1);
     lcd.print("BPM: ");
     lcd.setCursor(9,1);
     lcd.print(BPM);
     delay(2000);
    /*myservo1.write(94);
    delay(1);
    myservo1.write(14);
    delay(500);*/
     
    myservo1.write(10);
    delay(100);
    myservo1.write(100);
    delay(500);


    lcd.clear();

   if(BPM>120){
   lcd.clear();
    digitalWrite(1, HIGH);
    digitalWrite(3, HIGH);
     lcd.setCursor(2,0);
    lcd.print("Your BPM is");
     lcd.setCursor(0,1);
    lcd.print("    ! HIGH !     ");
    delay(2000);
    lcd.clear();
  delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Take long breathe");
   // lcd.write(3);
    delay(1000);
    lcd.clear();

    lcd.setCursor(5,0);
    lcd.print("Relax");
 //   lcd.write(3);
    delay(700);
    lcd.clear();

    lcd.setCursor(6,0);
    lcd.print("AND");
  //  lcd.write(3);
    delay(700);
    lcd.clear();

    lcd.setCursor(1,0);
    lcd.print("Calm Yourself");
  //  lcd.write(3);
    delay(700);
    lcd.clear();

    lcd.setCursor(6,0);
    lcd.print("AND");
  //  lcd.write(3);
    delay(700);
    lcd.clear();


    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Take tablets by");
    delay(600);
    lcd.setCursor(0,1);
    lcd.write(3);
    lcd.print(" DOCTOR DOOM ");
    lcd.write(3);
    delay(4000);
      lcd.setCursor(3, 0);
  lcd.print(" BPM:");
  lcd.print(BPM);
  Serial.print(BPM);
    myservo2.write(10);
    delay(100);
    myservo2.write(100);
    delay(500);
    digitalWrite(1, LOW);
     digitalWrite(3,LOW);
    lcd.clear();

    if(BPM<70){
   lcd.clear();
    digitalWrite(1, HIGH);
    digitalWrite(3, HIGH);
     lcd.setCursor(2,0);
    lcd.print("Your BPM is");
     lcd.setCursor(1,1);
    lcd.print("    ! LOW !    ");
    delay(2000);
    lcd.clear();
  delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Take long breathe");
   // lcd.write(3);
    delay(1000);
    lcd.clear();

    lcd.setCursor(5,0);
    lcd.print("Relax");
 //   lcd.write(3);
    delay(700);
    lcd.clear();

    lcd.setCursor(6,0);
    lcd.print("AND");
  //  lcd.write(3);
    delay(700);
    lcd.clear();

    lcd.setCursor(1,0);
    lcd.print("Calm Yourself");
  //  lcd.write(3);
    delay(700);
    lcd.clear();

    lcd.setCursor(6,0);
    lcd.print("AND");
  //  lcd.write(3);
    delay(700);
    lcd.clear();


    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Take tablets by");
    delay(600);
    lcd.setCursor(0,1);
    lcd.write(3);
    lcd.print(" DOCTOR DOOM ");
    lcd.write(3);
    delay(4000);
      lcd.setCursor(3, 0);
  lcd.print(" BPM:");
  lcd.print(BPM);
  Serial.print(BPM);
    myservo1.write(10);
    delay(100);
    myservo1.write(100);
    delay(500);
    digitalWrite(1, LOW);
     digitalWrite(3,LOW);
    lcd.clear();
 
   }
   

  if (QS == true) // A Heartbeat Was Found
    {     
      // BPM and IBI have been Determined
      // Quantified Self "QS" true when arduino finds a heartbeat
      serialOutputWhenBeatHappens(); // A Beat Happened, Output that to serial.     
      QS = false; // reset the Quantified Self flag for next time    
    }
     
  delay(200);
    }
   
}