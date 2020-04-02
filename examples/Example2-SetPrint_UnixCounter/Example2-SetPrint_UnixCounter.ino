/*
  Setting and reading UNIX counter from the RV-3028-C7 Real Time Clock
  By: Constantin Koch
  Date: 7/31/2019
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting my work? Give me a star!
  https://github.com/constiko/RV-3028_C7-Arduino_Library

  This example shows how to set the UNIX counter on the RTC, and how to read it.

*/

#include "RV-3028-C7.h"

RV3028 rtc;

//The below variable controls what the UNIX counter will be set to
#define UNIX 946684800 //GMT: Saturday, 1. January 2000 00:00:00

//https://www.epochconverter.com/


void setup() {

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Read/Write UNIX Counter - RTC Example");

  Wire.begin();
  if (rtc.begin() == false)
  {
    serialPrintError();
    while (1);
  }
  else
  {
    Serial.println("RTC online!");
    serialPintHelp();
  }
}

void loop()
{

  if (rtc.updateTime() == false) //Updates the time variables from RTC
  {
    serialPrintError();
  }
  else
  {
    String currentTime = rtc.stringTimeStamp();
    uint32_t currentUnixCounter = rtc.getUnixCounter();
    static uint32_t previousUnixCounter = currentUnixCounter;

    //print only every second
    if (currentUnixCounter != previousUnixCounter)
    {
      Serial.println(currentTime);
      Serial.print("Unix Counter: ");
      Serial.println(rtc.getUnixCounter());

      //Save last time
      previousUnixCounter = currentUnixCounter;
    }
  }

  if (Serial.available())
  {
    switch (Serial.read())
    {
      case 's':
        //Set UNIX counter to 1.1.2000 0:00:00
        if (rtc.setUnixCounter(UNIX) == false)
        {
          serialPrintError();
        }
        break;

      case '0':
        //Use this to set the Unix Counter to 0
        if (rtc.setUnixCounter(0) == false)
        {
          serialPrintError();
        }
        break;
      case '?':
        //Use this to print help for possible commands
        serialPintHelp();
        break;
    }
  }
}

void serialPintHelp()
{
  Serial.println(F("\'?\' print commands"));
  Serial.println(F("\'s\' set Unix Counter to GMT: Saturday, 1. January 2000 00:00:00"));
  Serial.println(F("\'0\' set Unix Counter to 0"));
}

void serialPrintError()
{
  Serial.println(F("Something went wrong!"));
}
