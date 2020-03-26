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
    Serial.println(F("Countdown Starts!"));
    //page 63
    //disable TE
    rtc.enableTimer(false);
    //disable TF
    rtc.clearTimerFlag();
    //disable TIE
    rtc.enableTimerInterrupt(false);
    //disable AIE
    rtc.enableAlarmInterrupt(false);
    //disable TRPT
    rtc.enableTimerRepeat(false);

    //Timer Clock Frequency TD
    //rtc.configureTimerFrequency(0);//244,14 micro useconds
    rtc.configureTimerFrequency(1);//15,625 milli seconds
    //rtc.configureTimerFrequency(2);//seconds
    //rtc.configureTimerFrequency(3);//minutes

    //uint16_t duration = 4095;//0
    uint16_t duration = 50;//1
    rtc.setTimerDuration(duration);

    Serial.print(F("CTRL1:\t"));
    Serial.println(rtc.readRegister(RV3028_CTRL1), BIN);
    Serial.print(F("CTRL2:\t"));
    Serial.println(rtc.readRegister(RV3028_CTRL2), BIN);
    Serial.print(F("STATUS:\t"));
    Serial.println(rtc.readRegister(RV3028_STATUS), BIN);
    Serial.print(F("Duration:\t"));
    Serial.println(rtc.getTimerDuration());
    Serial.println();

    //enable TE
    rtc.enableTimer(true);
    //enable TIE
    rtc.enableTimerInterrupt(true);
    

    Serial.print(F("CTRL1:\t"));
    Serial.println(rtc.readRegister(RV3028_CTRL1), BIN);
    Serial.print(F("CTRL2:\t"));
    Serial.println(rtc.readRegister(RV3028_CTRL2), BIN);
    Serial.print(F("STATUS:\t"));
    Serial.println(rtc.readRegister(RV3028_STATUS), BIN);
    Serial.println();
  }
}

void loop() {

  Serial.print(F("Countdown:\t"));
  Serial.println(rtc.getTimerCurrent());

  //countdown
  if (Serial.available())
  {
    switch (Serial.read())
    {
      case 'w':
        //enable countdown
        rtc.enableTimer(true);
        break;

      case 's':
        //disable countdown
        rtc.enableTimer(false);
        break;

      case 'd':
        //Repeat Mode TRPT
        rtc.enableTimerRepeat(true);
        break;

      case 'a':
        //Single Mode TRPT
        rtc.enableTimerRepeat(false);
        break;
    }
  }
}
