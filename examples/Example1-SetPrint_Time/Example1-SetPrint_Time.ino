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
uint8_t seconds  = 0;
uint8_t minutes  = 0;
const uint8_t hour    = 10;
const uint8_t day     = 5;// 0 = Sunday, 6 = Saturday
const uint8_t date    = 3;
const uint8_t month   = 4;
const uint16_t year   = 2020;

//The below variables control what the alarm will be set to
uint8_t alm_hour = 12;
uint8_t alm_minute = 43;
uint8_t alm_date_or_weekday = 2;
bool alm_isweekday = false;
uint8_t alm_mode = 7;
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


void setup()
{

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

    //Enable alarm
    //rtc.enableAlarmInterrupt(alm_minute, alm_hour, alm_date_or_weekday, alm_isweekday, alm_mode);
    //rtc.disableAlarmInterrupt();  //disables the interrupt (not the alarm flag)

    rtc.clearAlarmFlag();
    rtc.clearWeekdayAlarmFlag();
    rtc.enableUpdateInterrupt(false);
    rtc.clearUpdateFlag();
    rtc.enableTimer(false);
    rtc.clearStatus();//clears the complete status register

    serialPintHelp();
    serialPrintRegisters();
  }
}

void loop()
{
  //PRINT TIME
  if (rtc.updateTime() == false) //Updates the time variables from RTC
  {
    Serial.print(F("RTC failed to update"));
  }
  else
  {

    /*
        if (rtc.getUpdateFlag())
        {
           Serial.println(F("clearUpdateFlag"));
          rtc.clearUpdateFlag();
        }
    */
    String currentTime = rtc.stringTimeStamp();
    static String previousTime = currentTime;

    //print only every second
    if (currentTime != previousTime)
    {
      Serial.println(currentTime);
      //Save last time
      previousTime = currentTime;
    }
  }
  /*
    //Read Alarm Flag
    if (rtc.readAlarmFlag())
    {
      Serial.println(F("ALARM!!!!"));
      Serial.print(F("Status:\t"));
      Serial.println(rtc.readRegister(RV3028_STATUS), BIN);
      delay(5000);
      rtc.disableAlarmInterrupt();  //Only disables the interrupt
      rtc.clearAlarmFlag();         //disables the alarm flag
    }
  */

  //Set Time
  if (Serial.available())
  {
    switch (Serial.read())
    {
      case 's':
        //Use the time from the Arduino compiler (build time) to set the RTC
        //Keep in mind that Arduino does not get the new compiler time every time it compiles.
        //to ensure the proper time is loaded, open up a fresh version of the IDE and load the sketch.
        if (rtc.setToCompilerTime() == false)
        {
          serialPrintError();
        }
        break;

      case 't':
        //Use this to set the RTC with the predefined global values
        if (rtc.setTime(seconds, minutes, hour, day, date, month, year) == false)
        {
          serialPrintError();
        }
        break;

      case '0':
        //Use this to set the RTC seconds to 0
        if (rtc.setSeconds(0) == false)
        {
          serialPrintError();
        }
        break;

      case '+':
        //Use this to increase minutes
        minutes  = rtc.getMinutes();
        if (minutes == 59) minutes = 0;
        else minutes++;

        if (rtc.setMinutes(minutes) == false)
        {
          serialPrintError();
        }
        break;

      case '-':
        //Use this to decrease minutes
        minutes  = rtc.getMinutes();
        if (minutes == 0) minutes = 59;
        else minutes--;

        if (rtc.setMinutes(minutes) == false)
        {
          serialPrintError();
        }
        break;

      case 'r':
        //Use this to print registers
        serialPrintRegisters();
        break;

      case '?':
        //Use this to print help for possible commands
        serialPintHelp();
        break;
    }
  }
}
void serialPrintRegisters()
{
  Serial.print(F("STATUS:\t\t"));
  Serial.println(rtc.readRegister(RV3028_STATUS), BIN);
  Serial.print(F("CONTROL1:\t"));
  Serial.println(rtc.readRegister(RV3028_CTRL1), BIN);
  Serial.print(F("CONTROL2:\t"));
  Serial.println(rtc.readRegister(RV3028_CTRL2), BIN);
  Serial.print(F("EEPROM Backup:\t"));
  Serial.println(rtc.readRegister(EEPROM_Backup_Register), BIN);
}

void serialPrintError()
{
  Serial.println(F("Something went wrong setting the time"));
}

void serialPintHelp()
{
  Serial.println(F("\'?\' print commands"));
  Serial.println(F("\'r\' print register"));
  Serial.println(F("\'s\' set compile time"));
  Serial.println(F("\'t\' set preset time"));
  Serial.println(F("\'0\' set seconds to 0"));
  Serial.println(F("\'+\' increase minutes"));
  Serial.println(F("\'-\' decrease minutes"));
}
