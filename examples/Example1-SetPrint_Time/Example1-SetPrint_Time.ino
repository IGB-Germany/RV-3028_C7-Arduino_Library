/*
  Setting alarm interrupts at RV-3028-C7 Real Time Clock
  By: Constantin Koch
  Date: 7/31/2019
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting my work? Give me a star!
  https://github.com/constiko/RV-3028_C7-Arduino_Library

  This example shows how to set alarm interrupts at RV-3028-C7 Real Time Clock.

  Reworked to enable periodic countdown
  By: Marcus Bockting
  Date: 2/18/2020
  
  Changed: serial monitor at 9600 baud
  Changed: F macro
  New: read and print register RV3028_STATUS, RV3028_ID
  New: Alarm message
  New: Option to set time by predefined globals
  
*/

#include "RV-3028-C7.h"

RV3028 rtc;

//The below variables control what the date will be set to
const uint8_t sec     = 0;
const uint8_t minute  = 0;
const uint8_t hour    = 0;
const uint8_t day     = 0;
const uint8_t date    = 1;
const uint8_t month   = 1;
const uint16_t year   = 2020;

//The below variables control what the alarm will be set to
uint8_t alm_minute = 00;
uint8_t alm_hour = 12;
uint8_t alm_date_or_weekday = 2;
bool alm_isweekday = false;
uint8_t alm_mode = 4;
/*********************************
  Set the alarm mode in the following way:
  0: When minutes, hours and weekday/date match (once per weekday/date)
  1: When hours and weekday/date match (once per weekday/date)
  2: When minutes and weekday/date match (once per hour per weekday/date)
  3: When weekday/date match (once per weekday/date)
  4: When hours and minutes match (once per day)
  5: When hours match (once per day)
  6: When minutes match (once per hour)
  7: All disabled – Default value

  If you want to set a weekday alarm (alm_isweekday = true), set 'alm_date_or_weekday' from 0 (Sunday) to 6 (Saturday)
********************************/
void setup() {

  //Standard speed
  Serial.begin(9600);
  while (!Serial);
  Serial.println(F("Read/Write Time - RTC Example"));

  Wire.begin();
  if (rtc.begin() == false)
  {
    Serial.println(F("Something went wrong, check wiring"));
    while (1);
  }
  else
  {
    Serial.println(F("RTC online!"));
    Serial.print(F("HID:"));
    Serial.println(rtc.readRegister(RV3028_ID) >> 4);
    Serial.print(F("VID:"));
    Serial.println(rtc.readRegister(RV3028_ID) & 0b1111);
    //delay(1000);

    //Enable alarm interrupt
    rtc.enableAlarmInterrupt(alm_minute, alm_hour, alm_date_or_weekday, alm_isweekday, alm_mode);
    //rtc.disableAlarmInterrupt();  //Only disables the interrupt (not the alarm flag)
    Serial.print(F("Status:"));
    Serial.println(rtc.readRegister(RV3028_STATUS), BIN);
  }
}

void loop() {

  //PRINT TIME
  if (rtc.updateTime() == false) //Updates the time variables from RTC
  {
    Serial.print(F("RTC failed to update"));
  }
  else
  {
    String currentTime = rtc.stringTimeStamp();
    //Serial.println(currentTime + "     \'s\' = set compiler time");
    Serial.print(currentTime);
    Serial.println(F("\t\'s\' set compile time \'t\' reset"));

    Serial.print(F("Status:\t"));
    Serial.println(rtc.readRegister(RV3028_STATUS), BIN);
  }

  //Read Alarm Flag
  if (rtc.readAlarmInterruptFlag())
  {
    Serial.println(F("ALARM!!!!"));
    delay(5000);
    rtc.disableAlarmInterrupt();  //Only disables the interrupt (not the alarm flag)
  }

  //SET Time
  if (Serial.available())
  {
    switch (Serial.read())
    {
      case 's':
        //Use the time from the Arduino compiler (build time) to set the RTC
        //Keep in mind that Arduino does not get the new compiler time every time it compiles. to ensure the proper time is loaded, open up a fresh version of the IDE and load the sketch.
        if (rtc.setToCompilerTime() == false)
        {
          Serial.println(F("Something went wrong setting the time"));
        }
        break;

      case 't':
        //Use this to set the RTC with the predefined global values
        if (rtc.setTime(sec, minute, hour, day, date, month, year) == false)
        {
          Serial.println(F("Something went wrong setting the time"));
        }
        break;
    }
  }
}
