// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

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
const int trigPin = 7;
const int echoPin = 8;
const int ledPin = 9;
const int buzzPin = 10;

// defines variables
long duration;
int distance = 0; // in cm.
int prevDistance = 0; //saves the last distance from the last run through
int goAwayFlag = 0; //flags if the item has been there for more than 10 cycles
boolean isItBuzzing = false;
boolean showingLight = false;
int delayBetCheck = 500; // delay between scans
int timeStayed = 0; //time the object has been there

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
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
    
//    Serial.print(" since midnight 1/1/1970 = ");
//    Serial.print(now.unixtime());
//    Serial.print("s = ");
//    Serial.print(now.unixtime() / 86400L);
//    Serial.println("d");
}

void showEffect() {
    DateTime now = rtc.now(); 
    int currentHour = now.hour();
    Serial.print("now : ");
    Serial.print(currentHour);

    if (currentHour >= 19 && currentHour <= 7) {
        showingLight = true;
        Serial.print("now : ");
        digitalWrite(ledPin, HIGH);
        delay(1000);
        digitalWrite(ledPin, LOW);
        delay(100);
    } else {
        isItBuzzing = true;
        digitalWrite(buzzPin, HIGH);
        delay(1000);
        digitalWrite(buzzPin, LOW);
        delay(100);
    }
}

void loop() {
//  printTime();
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(delayBetCheck);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  //saves the previous distance
  prevDistance = distance;

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;

  //checks if the previous distance is the same as the new one
  if (abs(distance - prevDistance) <= 10) {
    goAwayFlag++;
  } else {
    goAwayFlag = 0;
  }

  if (distance < 250 && distance != 0 && goAwayFlag >= 10 && goAwayFlag <= 120) { //if the object has less than 2.5 meters away for 10 cycles, we flip out
    showEffect();
    timeStayed += delayBetCheck;
  } else {
    if (goAwayFlag > 120) {
      goAwayFlag = 0;
    }
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzPin, LOW);
    isItBuzzing = false;
    showingLight = false;
  }
  Serial.print("distance: ");
  Serial.print(distance);
  Serial.print(" , ");
  Serial.print("prevDistance: ");
  Serial.print(prevDistance);
  Serial.print(" , ");
  Serial.print("Is it buzzing? ");
  Serial.println(isItBuzzing);
  Serial.print("showing light? ");
  Serial.println(showingLight);
}
