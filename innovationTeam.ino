// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

#define DISTANCE 30
#define SENSORS_NUM 3

#if defined(ARDUINO_ARCH_SAMD)
   // for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setupClock() {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  // rtc.adjust(DateTime(2019, 5, 6, 9, 13, 0));
  
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    // rtc.adjust(DateTime(2019, 5, 6, 11, 10, 0));
  }
}

// defines pins numbers
const int trigPin[3] = { 2 , 4 , 7 };
const int echoPin[3] = { 3 , 5 ,8 };
const int ledPin = 9;
const int buzzPin = 10;

// defines variables
int distance[3] = { 0 , 0 , 0}; // in cm.
int prevDistance[3] = { 0 , 0 , 0}; //saves the last distance from the last run through
int goAwayFlag[3] = { 0 , 0 , 0}; //flags if the item has been there for more than 10 cycles
boolean isItBuzzing = false;
boolean showingLight = false;
int delayBetCheck = 500; // delay between scans
int timeStayed = 0; //time the object has been there

void setup() {
  for ( int i = 0 ; i < SENSORS_NUM ; i++ ){
  pinMode(trigPin[i], OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin[i], INPUT); // Sets the echoPin as an Input
  }
  pinMode(buzzPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); // Starts the serial communication
  setupClock();
}

void printTime() {
    DateTime now = rtc.now(); 
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
}

void showEffect() {
    DateTime now = rtc.now(); 
    int currentHour = now.hour();
    Serial.print("now : ");
    Serial.print(currentHour);

    if (currentHour >= 19 && currentHour <= 7 || true) {
        showingLight = true;
        Serial.print("now : ");
        digitalWrite(ledPin, HIGH);
        delay(500);
        digitalWrite(ledPin, LOW);
        delay(500);
    } else {
        isItBuzzing = true;
        tone(buzzPin, 1000);
        delay(1000);
        noTone(buzzPin);
        delay(1000);
    }
}

int getDistance(int trigPin, int echoPin)
{
  long duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  duration = duration / 59;
  Serial.print("Distance is: ");
  Serial.print(duration);
  Serial.println(" centimeters");
  return duration;
}

void loop() {

for (int i = 0 ; i < SENSORS_NUM ; i++  ){
  //saves the previous distance
  prevDistance[i] = distance[i];
  // Calculating the distance
  distance[i] = getDistance(trigPin[i],echoPin[i]);

  //checks if the previous distance is the same as the new one
  if (abs(distance[i] - prevDistance[i]) <= 10) {
    goAwayFlag[i]++;
  } else {
    goAwayFlag[i] = 0;
  }

  if (distance[i] < DISTANCE && distance[i] != 0 && goAwayFlag[i] >= 10 && goAwayFlag[i] <= 120) { //if the object has less than 2.5 meters away for 10 cycles, we flip out
    showEffect();
    timeStayed += delayBetCheck;
  } else {
    if (goAwayFlag[i] > 120) {
      goAwayFlag[i] = 0;
    }
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzPin, LOW);
    isItBuzzing = false;
    showingLight = false;
  }
  Serial.print("Sensor no.");
  Serial.print(i);
  Serial.println("'s status:");
  Serial.print("distance: ");
  Serial.print(distance[i]);
  Serial.print(" , ");
  Serial.print("prevDistance: ");
  Serial.print(prevDistance[i]);
  Serial.print(" , ");
  Serial.print("Is it buzzing? ");
  Serial.println(isItBuzzing);
  Serial.print("showing light? ");
  Serial.println(showingLight);
  }
}
