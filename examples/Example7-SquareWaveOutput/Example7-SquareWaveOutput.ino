/*
  Setting PROGRAMMABLE CLOCK OUTPUT/square wave output at RV-3028-C7 Real Time Clock
  By: IGB
  Date: 3/31/20
  License: unlicensed

  This example shows how to set PROGRAMMABLE CLOCK OUTPUT/square wave output at RV-3028-C7 Real Time Clock.
  Many usages of square wave are possible.
  Time is the universal synchronizer among peripherals.

  Six different frequencies or the countdown timer interrupt signal can be output on CLKOUT pin.
  The signal selection is done in the FD field (EEPROM 35h).

  The frequencies available:

  Divided Xtal oscillator frequencies, digitally compensated according
  to the oscillator offset value EEOffset (EEPROM 36h and 37h):
  1 Hz, 32 Hz, 64 Hz, 1024 Hz, 8192 Hz

  Direct from Xtal oscillator, not offset compensated:
  32768 Hz

*/

#include "RV-3028-C7.h"

RV3028 rtc;

void setup()
{

  //Standard speed
  Serial.begin(9600);
  while (!Serial);
  Serial.println(F("PROGRAMMABLE CLOCK OUTPUT - RTC Example"));

  Wire.begin();
  if (rtc.begin() == false)
  {
    Serial.println(F("Something went wrong, check wiring"));
    while (1);
  }
  else
  {
    Serial.println(F("Registers:"));
    Serial.print(F("STATUS:\t"));
    Serial.println(rtc.readRegister(RV3028_STATUS), BIN);
    printRegister_EEPROM_CLKOUT();
  }
}

void loop() {

  if (Serial.available())
  {
    switch (Serial.read())
    {
      //Clock Output
      case 'w':
        //enable
        rtc.enableClockOutput(true);
        printRegister_EEPROM_CLKOUT();
        break;

      case 's':
        //disable
        rtc.enableClockOutput(false);
        printRegister_EEPROM_CLKOUT();
        break;

      //Syncronize
      case 'd':
        //enable
        rtc.enableClockOutputSynchronize(true);
        printRegister_EEPROM_CLKOUT();
        break;
      case 'a':
        //disable
        rtc.enableClockOutputSynchronize(false);
        printRegister_EEPROM_CLKOUT();
        break;

      //Frequency
      case '0':
        rtc.setClockOutputFrequency(FREQ_32768);
        printRegister_EEPROM_CLKOUT();
        break;
      case '1':
        rtc.setClockOutputFrequency(FREQ_8192);
        printRegister_EEPROM_CLKOUT();
        break;
      case '2':
        rtc.setClockOutputFrequency(FREQ_1024);
        printRegister_EEPROM_CLKOUT();
        break;
      case '3':
        rtc.setClockOutputFrequency(FREQ_64);
        printRegister_EEPROM_CLKOUT();
        break;
      case '4':
        rtc.setClockOutputFrequency(FREQ_32);
        printRegister_EEPROM_CLKOUT();
        break;
      case '5':
        rtc.setClockOutputFrequency(FREQ_1);
        printRegister_EEPROM_CLKOUT();
        break;

      case 'p':
        rtc.setClockOutputFrequency(FREQ_PRE);
        printRegister_EEPROM_CLKOUT();
        break;
      case 'o':
        rtc.setClockOutputFrequency(FREQ_OFF);
        printRegister_EEPROM_CLKOUT();
        break;


      //POWER ON RESET INTERRUPT FUNCTION
      case 'e':
        //enable
        rtc.enablePowerOnResetInterrupt(true);
        printRegister_EEPROM_CLKOUT();
        break;
      case 'q':
        //disable
        rtc.enablePowerOnResetInterrupt(false);
        printRegister_EEPROM_CLKOUT();
        break;
    }
  }
}

void printRegister_EEPROM_CLKOUT()
{
  Serial.print(F("RV3028_EEPROM_CLKOUT:\t"));
  Serial.println(rtc.readRegister(RV3028_EEPROM_CLKOUT), BIN);
}
