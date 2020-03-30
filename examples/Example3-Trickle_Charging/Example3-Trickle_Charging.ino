/*
  Trickle Charge Configuration of RV-3028-C7 Real Time Clock
  By: Constantin Koch
  Date: 7/31/2019
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting my work? Give me a star!
  https://github.com/constiko/RV-3028_C7-Arduino_Library

  This example shows how to set the Trickle Charge Configuration of RTC RV-3028-C7.
  Open the serial monitor at 115200 baud.
*/

#include "RV-3028-C7.h"

RV3028 rtc;

void setup() {

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Trickle Charging - RTC Example");

  Wire.begin();
  if (rtc.begin() == false) {
    Serial.println("Something went wrong, check wiring");
    while (1);
  }
  else
    Serial.println("RTC online!");
  
  Serial.println();
  Serial.print(F("EEPROM Backup:\t"));
  Serial.println(rtc.readRegister(EEPROM_Backup_Register), BIN);

  rtc.enableBackupSwitchoverInterrupt(false);//No interrupt signal is generated when Backup Switchover occurs
  rtc.enableTrickleCharge(true);   //Trickle Charger enabled resistor default 3k
  //rtc.setTrickleChargeResistor(TCR_5K); //series resistor 5kOhm
  rtc.setTrickleChargeResistor(TCR_9K); //series resistor 9kOhm
  //rtc.setTrickleChargeResistor(TCR_15K); //series resistor 15kOhm
  rtc.setBackupSwitchoverMode(BSM_DISABLED); //Backup Switchover disabled
  
  Serial.print(F("EEPROM Backup:\t"));
  Serial.println(rtc.readRegister(EEPROM_Backup_Register), BIN);

  rtc.enableBackupSwitchoverInterrupt(true);//interrupt signal is generated when Backup Switchover occurs
  rtc.enableTrickleCharge(false); //Trickle Charger disabled
  rtc.setBackupSwitchoverMode(BSM_LEVEL);//Backup Switchover enabled
  
  Serial.print(F("EEPROM Backup:\t"));
  Serial.println(rtc.readRegister(EEPROM_Backup_Register), BIN);
}

void loop() {

}
