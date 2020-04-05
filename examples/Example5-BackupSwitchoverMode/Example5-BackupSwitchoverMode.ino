/*
  Backup Switchover Mode of RV-3028-C7 Real Time Clock
  By: Constantin Koch
  Date: 7/31/2019
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting my work? Give me a star!
  https://github.com/constiko/RV-3028_C7-Arduino_Library

  This example shows how to set the Backup Switchover Mode of RTC RV-3028-C7.
  Open the serial monitor at 9600 baud.
  For more information see https://www.microcrystal.com/fileadmin/Media/Products/RTC/App.Manual/RV-3028-C7_App-Manual.pdf#page=43

*/

#include "RV-3028-C7.h"

RV3028 rtc;

void setup() {

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Backup Switchover Mode - RTC Example");

  Wire.begin();
  if (rtc.begin() == false) {
    Serial.println("Something went wrong, check wiring");
    while (1);
  }
  else
    Serial.println("RTC online!");
  Serial.println();
}

void loop()
{

  if (Serial.available())
  {
    switch (Serial.read())
    {
      case 'x':
        rtc.setBackupSwitchoverMode(BSM_DISABLED);
        Serial.println(F("Switchover disabled\t"));
        Serial.print(F("EEPROM Backup:\t"));
        Serial.println(rtc.readRegister(EEPROM_Backup_Register), BIN);
        break;

      case 'd':
        rtc.setBackupSwitchoverMode(BSM_DIRECT);
        Serial.println(F("Direct Switching Mode enabled"));
        Serial.println(F("VDD < VBACKUP"));
        Serial.print(F("EEPROM Backup:\t"));
        Serial.println(rtc.readRegister(EEPROM_Backup_Register), BIN);
        break;

      case 's':
        rtc.setBackupSwitchoverMode(BSM_LEVEL);
        Serial.println(F("Level Switching Mode enabled"));
        Serial.println(F("VDD < VTH (2.0 V)"));
        Serial.print(F("EEPROM Backup:\t"));
        Serial.println(rtc.readRegister(EEPROM_Backup_Register), BIN);
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
  Serial.println(F("\'d\' Set Direct Switching Mode"));
  Serial.println(F("\'s\' Set Level Switching Mode"));
  Serial.println(F("\'x\' Disable Switchover"));
}
