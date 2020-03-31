/******************************************************************************
  RV-3028-C7.h
  RV-3028-C7 Arduino Library
  Constantin Koch
  July 25, 2019
  https://github.com/constiko/RV-3028_C7-Arduino_Library

  Development environment specifics:
  Arduino IDE 1.8.9

  This code is released under the [MIT License](http://opensource.org/licenses/MIT).
  Please review the LICENSE.md file included with this example. If you have any questions
  or concerns with licensing, please contact constantinkoch@outlook.com.
  Distributed as-is; no warranty is given.
******************************************************************************/

#include "RV-3028-C7.h"

//****************************************************************************//
//
//  Settings and configuration
//
//****************************************************************************//

// Parse the __DATE__ predefined macro to generate date defaults:
// __Date__ Format: MMM DD YYYY (First D may be a space if <10)
// <MONTH>
#define BUILD_MONTH_JAN ((__DATE__[0] == 'J') && (__DATE__[1] == 'a')) ? 1 : 0
#define BUILD_MONTH_FEB (__DATE__[0] == 'F') ? 2 : 0
#define BUILD_MONTH_MAR ((__DATE__[0] == 'M') && (__DATE__[1] == 'a') && (__DATE__[2] == 'r')) ? 3 : 0
#define BUILD_MONTH_APR ((__DATE__[0] == 'A') && (__DATE__[1] == 'p')) ? 4 : 0
#define BUILD_MONTH_MAY ((__DATE__[0] == 'M') && (__DATE__[1] == 'a') && (__DATE__[2] == 'y')) ? 5 : 0
#define BUILD_MONTH_JUN ((__DATE__[0] == 'J') && (__DATE__[1] == 'u') && (__DATE__[2] == 'n')) ? 6 : 0
#define BUILD_MONTH_JUL ((__DATE__[0] == 'J') && (__DATE__[1] == 'u') && (__DATE__[2] == 'l')) ? 7 : 0
#define BUILD_MONTH_AUG ((__DATE__[0] == 'A') && (__DATE__[1] == 'u')) ? 8 : 0
#define BUILD_MONTH_SEP (__DATE__[0] == 'S') ? 9 : 0
#define BUILD_MONTH_OCT (__DATE__[0] == 'O') ? 10 : 0
#define BUILD_MONTH_NOV (__DATE__[0] == 'N') ? 11 : 0
#define BUILD_MONTH_DEC (__DATE__[0] == 'D') ? 12 : 0
#define BUILD_MONTH BUILD_MONTH_JAN | BUILD_MONTH_FEB | BUILD_MONTH_MAR | \
  BUILD_MONTH_APR | BUILD_MONTH_MAY | BUILD_MONTH_JUN | \
  BUILD_MONTH_JUL | BUILD_MONTH_AUG | BUILD_MONTH_SEP | \
  BUILD_MONTH_OCT | BUILD_MONTH_NOV | BUILD_MONTH_DEC
// <DATE>
#define BUILD_DATE_0 ((__DATE__[4] == ' ') ? 0 : (__DATE__[4] - 0x30))
#define BUILD_DATE_1 (__DATE__[5] - 0x30)
#define BUILD_DATE ((BUILD_DATE_0 * 10) + BUILD_DATE_1)
// <YEAR>
#define BUILD_YEAR (((__DATE__[7] - 0x30) * 1000) + ((__DATE__[8] - 0x30) * 100) + \
                    ((__DATE__[9] - 0x30) * 10)  + ((__DATE__[10] - 0x30) * 1))

// Parse the __TIME__ predefined macro to generate time defaults:
// __TIME__ Format: HH:MM:SS (First number of each is padded by 0 if <10)
// <HOUR>
#define BUILD_HOUR_0 ((__TIME__[0] == ' ') ? 0 : (__TIME__[0] - 0x30))
#define BUILD_HOUR_1 (__TIME__[1] - 0x30)
#define BUILD_HOUR ((BUILD_HOUR_0 * 10) + BUILD_HOUR_1)
// <MINUTE>
#define BUILD_MINUTE_0 ((__TIME__[3] == ' ') ? 0 : (__TIME__[3] - 0x30))
#define BUILD_MINUTE_1 (__TIME__[4] - 0x30)
#define BUILD_MINUTE ((BUILD_MINUTE_0 * 10) + BUILD_MINUTE_1)
// <SECOND>
#define BUILD_SECOND_0 ((__TIME__[6] == ' ') ? 0 : (__TIME__[6] - 0x30))
#define BUILD_SECOND_1 (__TIME__[7] - 0x30)
#define BUILD_SECOND ((BUILD_SECOND_0 * 10) + BUILD_SECOND_1)

RV3028::RV3028(void)
  : _time{0},
    _i2cPort(NULL)
{

}

boolean RV3028::begin(TwoWire &wirePort)
{
  //We require caller to begin their I2C port, with the speed of their choice
  //external to the library
  //_i2cPort->begin();
  _i2cPort = &wirePort;

  set24Hour();
  delay(1);
  enableTrickleCharge(false);
  delay(1);

  return (setBackupSwitchoverMode(3) && writeRegister(RV3028_STATUS, 0x00));
}

bool RV3028::setTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t weekday, uint8_t date, uint8_t month, uint16_t year)
{
  _time[TIME_SECONDS] = DECtoBCD(sec);
  _time[TIME_MINUTES] = DECtoBCD(min);
  _time[TIME_HOURS] = DECtoBCD(hour);
  _time[TIME_WEEKDAY] = DECtoBCD(weekday);
  _time[TIME_DATE] = DECtoBCD(date);
  _time[TIME_MONTH] = DECtoBCD(month);
  _time[TIME_YEAR] = DECtoBCD(year - 2000);

  bool status = false;

  if (is12Hour())
  {
    set24Hour();
    status = setTime(_time, TIME_ARRAY_LENGTH);
    set12Hour();
  }
  else
  {
    status = setTime(_time, TIME_ARRAY_LENGTH);
  }
  return status;
}

// setTime -- Set time and date/day registers of RV3028 (using data array)
bool RV3028::setTime(uint8_t * time, uint8_t len)
{
  if (len != TIME_ARRAY_LENGTH)
    return false;

  return writeMultipleRegisters(RV3028_SECONDS, time, len);
}

bool RV3028::setSeconds(uint8_t value)
{
  _time[TIME_SECONDS] = DECtoBCD(value);
  return setTime(_time, TIME_ARRAY_LENGTH);
}

bool RV3028::setMinutes(uint8_t value)
{
  _time[TIME_MINUTES] = DECtoBCD(value);
  return setTime(_time, TIME_ARRAY_LENGTH);
}

bool RV3028::setHours(uint8_t value)
{
  _time[TIME_HOURS] = DECtoBCD(value);
  return setTime(_time, TIME_ARRAY_LENGTH);
}

bool RV3028::setWeekday(uint8_t value)
{
  _time[TIME_WEEKDAY] = DECtoBCD(value);
  return setTime(_time, TIME_ARRAY_LENGTH);
}

bool RV3028::setDate(uint8_t value)
{
  _time[TIME_DATE] = DECtoBCD(value);
  return setTime(_time, TIME_ARRAY_LENGTH);
}

bool RV3028::setMonth(uint8_t value)
{
  _time[TIME_MONTH] = DECtoBCD(value);
  return setTime(_time, TIME_ARRAY_LENGTH);
}

bool RV3028::setYear(uint16_t value)
{
  _time[TIME_YEAR] = DECtoBCD(value - 2000);
  return setTime(_time, TIME_ARRAY_LENGTH);
}

//Takes the time from the last build and uses it as the current time
//Works very well as an arduino sketch
bool RV3028::setToCompilerTime()
{
  _time[TIME_SECONDS] = DECtoBCD(BUILD_SECOND);
  _time[TIME_MINUTES] = DECtoBCD(BUILD_MINUTE);
  _time[TIME_HOURS] = DECtoBCD(BUILD_HOUR);

  //Build_Hour is 0-23, convert to 1-12 if needed
  if (is12Hour())
  {
    uint8_t hour = BUILD_HOUR;

    boolean pm = false;

    if (hour == 0)
      hour += 12;
    else if (hour == 12)
      pm = true;
    else if (hour > 12)
    {
      hour -= 12;
      pm = true;
    }

    _time[TIME_HOURS] = DECtoBCD(hour); //Load the modified hours

    if (pm == true) _time[TIME_HOURS] |= (1 << HOURS_AM_PM); //Set AM/PM bit if needed
  }

  // Calculate weekday (from here: http://stackoverflow.com/a/21235587)
  // 0 = Sunday, 6 = Saturday
  uint16_t d = BUILD_DATE;
  uint16_t m = BUILD_MONTH;
  uint16_t y = BUILD_YEAR;
  uint16_t weekday = (d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) % 7 + 1;
  _time[TIME_WEEKDAY] = DECtoBCD(weekday);

  _time[TIME_DATE] = DECtoBCD(BUILD_DATE);
  _time[TIME_MONTH] = DECtoBCD(BUILD_MONTH);
  _time[TIME_YEAR] = DECtoBCD(BUILD_YEAR - 2000); //! Not Y2K (or Y2.1K)-proof :(

  return setTime(_time, TIME_ARRAY_LENGTH);
}

//Move the hours, mins, sec, etc registers from RV-3028-C7 into the _time array
//Needs to be called before printing time or date
//We do not protect the GPx registers. They will be overwritten. The user has plenty of RAM if they need it.
bool RV3028::updateTime()
{
  if (readMultipleRegisters(RV3028_SECONDS, _time, TIME_ARRAY_LENGTH) == false)
    return (false); //Something went wrong

  if (is12Hour()) _time[TIME_HOURS] &= ~(1 << HOURS_AM_PM); //Remove this bit from value

  return true;
}

//Returns a pointer to array of chars that are the date in mm/dd/yyyy format because they're weird
char* RV3028::stringDateUSA()
{
  static char date[11]; //Max of mm/dd/yyyy with \0 terminator
  sprintf(date, "%02hhu/%02hhu/20%02hhu", BCDtoDEC(_time[TIME_MONTH]), BCDtoDEC(_time[TIME_DATE]), BCDtoDEC(_time[TIME_YEAR]));
  return (date);
}

//Returns a pointer to array of chars that are the date in dd/mm/yyyy format
char*  RV3028::stringDate()
{
  static char date[11]; //Max of dd/mm/yyyy with \0 terminator
  sprintf(date, "%02hhu/%02hhu/20%02hhu", BCDtoDEC(_time[TIME_DATE]), BCDtoDEC(_time[TIME_MONTH]), BCDtoDEC(_time[TIME_YEAR]));
  return (date);
}

//Returns a pointer to array of chars that represents the time in hh:mm:ss format
//Adds AM/PM if in 12 hour mode
char* RV3028::stringTime()
{
  static char time[11]; //Max of hh:mm:ssXM with \0 terminator

  if (is12Hour() == true)
  {
    char half = 'A';
    if (isPM()) half = 'P';

    sprintf(time, "%02hhu:%02hhu:%02hhu%cM", BCDtoDEC(_time[TIME_HOURS]), BCDtoDEC(_time[TIME_MINUTES]), BCDtoDEC(_time[TIME_SECONDS]), half);
  }
  else
    sprintf(time, "%02hhu:%02hhu:%02hhu", BCDtoDEC(_time[TIME_HOURS]), BCDtoDEC(_time[TIME_MINUTES]), BCDtoDEC(_time[TIME_SECONDS]));

  return (time);
}

char* RV3028::stringTimeStamp()
{
  static char timeStamp[25]; //Max of yyyy-mm-ddThh:mm:ss.ss with \0 terminator

  if (is12Hour() == true)
  {
    char half = 'A';
    if (isPM()) half = 'P';

    sprintf(timeStamp, "20%02hhu-%02hhu-%02hhu  %02hhu:%02hhu:%02hhu%cM", BCDtoDEC(_time[TIME_YEAR]), BCDtoDEC(_time[TIME_MONTH]), BCDtoDEC(_time[TIME_DATE]), BCDtoDEC(_time[TIME_HOURS]), BCDtoDEC(_time[TIME_MINUTES]), BCDtoDEC(_time[TIME_SECONDS]), half);
  }
  else
    sprintf(timeStamp, "20%02hhu-%02hhu-%02hhu  %02hhu:%02hhu:%02hhu", BCDtoDEC(_time[TIME_YEAR]), BCDtoDEC(_time[TIME_MONTH]), BCDtoDEC(_time[TIME_DATE]), BCDtoDEC(_time[TIME_HOURS]), BCDtoDEC(_time[TIME_MINUTES]), BCDtoDEC(_time[TIME_SECONDS]));

  return (timeStamp);
}

uint8_t RV3028::getSeconds()
{
  return BCDtoDEC(_time[TIME_SECONDS]);
}

uint8_t RV3028::getMinutes()
{
  return BCDtoDEC(_time[TIME_MINUTES]);
}

uint8_t RV3028::getHours()
{
  return BCDtoDEC(_time[TIME_HOURS]);
}

uint8_t RV3028::getWeekday()
{
  return BCDtoDEC(_time[TIME_WEEKDAY]);
}

uint8_t RV3028::getDate()
{
  return BCDtoDEC(_time[TIME_DATE]);
}

uint8_t RV3028::getMonth()
{
  return BCDtoDEC(_time[TIME_MONTH]);
}

uint16_t RV3028::getYear()
{
  return BCDtoDEC(_time[TIME_YEAR]) + 2000;
}

//Returns true if RTC has been configured for 12 hour mode
bool RV3028::is12Hour()
{
  uint8_t controlRegister2 = readRegister(RV3028_CTRL2);
  return (controlRegister2 & (1 << CTRL2_12_24));
}

//Returns true if RTC has PM bit set and 12Hour bit set
bool RV3028::isPM()
{
  uint8_t hourRegister = readRegister(RV3028_HOURS);
  if (is12Hour() && (hourRegister & (1 << HOURS_AM_PM)))
    return (true);
  return (false);
}

//Configure RTC to output 1-12 hours
//Converts any current hour setting to 12 hour
void RV3028::set12Hour()
{
  //Do we need to change anything?
  if (is12Hour() == false)
  {
    uint8_t hour = BCDtoDEC(readRegister(RV3028_HOURS)); //Get the current hour in the RTC

    //Set the 12/24 hour bit
    uint8_t setting = readRegister(RV3028_CTRL2);
    setting |= (1 << CTRL2_12_24);
    writeRegister(RV3028_CTRL2, setting);

    //Take the current hours and convert to 12, complete with AM/PM bit
    boolean pm = false;

    if (hour == 0)
      hour += 12;
    else if (hour == 12)
      pm = true;
    else if (hour > 12)
    {
      hour -= 12;
      pm = true;
    }

    hour = DECtoBCD(hour); //Convert to BCD

    if (pm == true) hour |= (1 << HOURS_AM_PM); //Set AM/PM bit if needed

    writeRegister(RV3028_HOURS, hour); //Record this to hours register
  }
}

//Configure RTC to output 0-23 hours
//Converts any current hour setting to 24 hour
void RV3028::set24Hour()
{
  //Do we need to change anything?
  if (is12Hour() == true)
  {
    //Not sure what changing the CTRL2 register will do to hour register so let's get a copy
    uint8_t hour = readRegister(RV3028_HOURS); //Get the current 12 hour formatted time in BCD
    boolean pm = false;
    if (hour & (1 << HOURS_AM_PM)) //Is the AM/PM bit set?
    {
      pm = true;
      hour &= ~(1 << HOURS_AM_PM); //Clear the bit
    }

    //Change to 24 hour mode
    uint8_t setting = readRegister(RV3028_CTRL2);
    setting &= ~(1 << CTRL2_12_24); //Clear the 12/24 hr bit
    writeRegister(RV3028_CTRL2, setting);

    //Given a BCD hour in the 1-12 range, make it 24
    hour = BCDtoDEC(hour); //Convert core of register to DEC

    if (pm == true) hour += 12; //2PM becomes 14
    if (hour == 12) hour = 0; //12AM stays 12, but should really be 0
    if (hour == 24) hour = 12; //12PM becomes 24, but should really be 12

    hour = DECtoBCD(hour); //Convert to BCD

    writeRegister(RV3028_HOURS, hour); //Record this to hours register
  }
}

//ATTENTION: Real Time and UNIX Time are INDEPENDENT!
bool RV3028::setUNIX(uint32_t value)
{
  uint8_t unix_reg[4];
  unix_reg[0] = value;
  unix_reg[1] = value >> 8;
  unix_reg[2] = value >> 16;
  unix_reg[3] = value >> 24;

  return writeMultipleRegisters(RV3028_UNIX_TIME0, unix_reg, 4);
}

//ATTENTION: Real Time and UNIX Time are INDEPENDENT!
uint32_t RV3028::getUNIX()
{
  uint8_t unix_reg[4];
  readMultipleRegisters(RV3028_UNIX_TIME0, unix_reg, 4);
  return ((uint32_t)unix_reg[3] << 24) | ((uint32_t)unix_reg[2] << 16) | ((uint32_t)unix_reg[1] << 8) | unix_reg[0];
}

/*********************************
  Set the alarm mode in the following way:
  0: When minutes, hours and weekday/date match (once per weekday/date)
  1: When hours and weekday/date match (once per weekday/date)
  2: When minutes and weekday/date match (once per hour per weekday/date)
  3: When weekday/date match (once per weekday/date)
  4: When hours and minutes match (once per day)
  5: When hours match (once per day)
  6: When minutes match (once per hour)
  7: All disabled � Default value
  If you want to set a weekday alarm (setWeekdayAlarm_not_Date = true), set 'date_or_weekday' from 0 (Sunday) to 6 (Saturday)
********************************/
void RV3028::enableAlarmInterrupt(uint8_t min, uint8_t hour, uint8_t date_or_weekday, bool setWeekdayAlarm_not_Date, uint8_t mode)
{
  //disable Alarm Interrupt to prevent accidental interrupts during configuration
  enableAlarmInterrupt(false);
  //clearStatus();
  clearAlarmFlag();

  //ENHANCEMENT: Add Alarm in 12 hour mode
  set24Hour();

  //Set WADA bit (Weekday/Date Alarm)
  uint8_t value = readRegister(RV3028_CTRL1);
  if (setWeekdayAlarm_not_Date)
    value &= ~(1 << CTRL1_WADA);
  else
    value |= 1 << CTRL1_WADA;
  writeRegister(RV3028_CTRL1, value);

  //Write alarm settings in registers 0x07 to 0x09
  uint8_t alarmTime[3];
  alarmTime[0] = DECtoBCD(min);				//minutes
  alarmTime[1] = DECtoBCD(hour);				//hours
  alarmTime[2] = DECtoBCD(date_or_weekday);	//date or weekday

  //shift alarm enable bits
  if (mode > 0b111) mode = 0b111; //0 to 7 is valid
  if (mode & 0b001)
    alarmTime[0] |= 1 << MINUTESALM_AE_M;
  if (mode & 0b010)
    alarmTime[1] |= 1 << HOURSALM_AE_H;
  if (mode & 0b100)
    alarmTime[2] |= 1 << DATE_AE_WD;
  //Write registers
  writeMultipleRegisters(RV3028_MINUTES_ALM, alarmTime, 3);

  //enable Alarm Interrupt
  enableAlarmInterrupt(true);
}

//Alarm Interrupt Enable AIE
void RV3028::enableAlarmInterrupt(bool enable)
{
  //change only AIE in CTRL2
  uint8_t reg = readRegister(RV3028_CTRL2);
  if (enable)
    writeRegister(RV3028_CTRL2, reg | 0b00001000);//set bit 4
  //reg |= (1 << CTRL2_AIE);
  else
    writeRegister(RV3028_CTRL2, reg & 0b11110111);//unset bit 4
}

uint8_t RV3028::readAlarmFlag()
{
  uint8_t stat = readStatus();
  return (stat & (1 << STATUS_AF));
}

void RV3028::clearAlarmFlag()
{
  //change only AF in STATUS
  uint8_t reg = readRegister(RV3028_STATUS);
  writeRegister(RV3028_STATUS, reg & 0b11111011);
}

uint8_t RV3028::readWeekdayAlarmFlag()
{
  uint8_t reg = readRegister(RV3028_CTRL1);
  return (reg & (1 << CTRL1_WADA));
}
void RV3028::clearWeekdayAlarmFlag()
{
  //change only TF in STATUS
  uint8_t reg = readRegister(RV3028_CTRL1);
  writeRegister(RV3028_CTRL1, reg & 0b11011111);//unset bit 6
}

void RV3028::enableClockOutput(bool enable)
{
  //Read EEPROM CLKOUT REGISTER 0x35
  uint8_t reg = readConfigEEPROM_RAMmirror(RV3028_EEPROM_CLKOUT);
  //register availabe ?
  if (reg == 0xFF) return;

  if (enable)
    writeConfigEEPROM_RAMmirror(RV3028_EEPROM_CLKOUT, reg |= (1 << EEPROM_CLKOUT_CLKOE));//set bit 7
  else
    writeConfigEEPROM_RAMmirror(RV3028_EEPROM_CLKOUT, reg &= ~(1 << EEPROM_CLKOUT_CLKOE));//unset bit 7
}

void RV3028::setClockOutputFrequency(uint8_t fd)
{
  //FD 000-111 in EEPROM CLKOUT REGISTER 0x35
  //out of range ?
  if (fd > 0b111) fd = FREQ_32768;

  //Read EEPROM CLKOUT REGISTER 0x35
  uint8_t reg = readConfigEEPROM_RAMmirror(RV3028_EEPROM_CLKOUT);
  //register availabe ?
  if (reg == 0xFF)
  {

    return;
  }

  reg = reg & 0b11111000;//Clear bits
  reg = reg | fd;//Write FD bits
  writeConfigEEPROM_RAMmirror(RV3028_EEPROM_CLKOUT, reg);
}

void RV3028::enableClockOutputSynchronize(bool enable)
{
  //Read EEPROM CLKOUT REGISTER 0x35
  uint8_t reg = readConfigEEPROM_RAMmirror(RV3028_EEPROM_CLKOUT);
  //register availabe ?
  if (reg == 0xFF) return;

  if (enable)
    writeConfigEEPROM_RAMmirror(RV3028_EEPROM_CLKOUT, reg |= (1 << EEPROM_CLKOUT_CLKSY));//set bit
  else
    writeConfigEEPROM_RAMmirror(RV3028_EEPROM_CLKOUT, reg &= ~(1 << EEPROM_CLKOUT_CLKSY));//unset bit
}

void RV3028::enablePowerOnResetInterrupt(bool enable)
{
  //Read EEPROM CLKOUT REGISTER 0x35
  uint8_t reg = readConfigEEPROM_RAMmirror(RV3028_EEPROM_CLKOUT);
  //register availabe ?
  if (reg == 0xFF) return;

  if (enable)
    writeConfigEEPROM_RAMmirror(RV3028_EEPROM_CLKOUT, reg |= (1 << EEPROM_CLKOUT_PORIE));//set bit
  else
    writeConfigEEPROM_RAMmirror(RV3028_EEPROM_CLKOUT, reg &= ~(1 << EEPROM_CLKOUT_PORIE));//unset bit
}

/*********************************
  Enable the Trickle Charger and set the Trickle Charge series resistor (default is 11k)
  TCR_3K  =  3kOhm
  TCR_5K  =  5kOhm
  TCR_9K  =  9kOhm
  TCR_15K = 15kOhm
*********************************/
void RV3028::enableTrickleCharge(bool enable)
{
  //Read EEPROM Backup Register (0x37)
  uint8_t reg = readConfigEEPROM_RAMmirror(EEPROM_Backup_Register);
  //register availabe ?
  if (reg == 0xFF) return;

  if (enable)
    writeConfigEEPROM_RAMmirror(EEPROM_Backup_Register, reg |= (1 << EEPROMBackup_TCE));//set bit 6
  else
    writeConfigEEPROM_RAMmirror(EEPROM_Backup_Register, reg &= ~(1 << EEPROMBackup_TCE));//unset bit 6
}

void RV3028::setTrickleChargeResistor(uint8_t tcr)
{
  //out of range ?
  if (tcr > 3) tcr = TCR_15K;

  //Read EEPROM Backup Register (0x37)
  uint8_t reg = readConfigEEPROM_RAMmirror(EEPROM_Backup_Register);
  //register availabe ?
  if (reg == 0xFF) return;

  reg &= EEPROMBackup_TCR_CLEAR;    //Clear TCR Bits
  reg |= tcr << EEPROMBackup_TCR_SHIFT; //Shift values into EEPROM Backup Register
  writeConfigEEPROM_RAMmirror(EEPROM_Backup_Register, reg);
}

bool RV3028::setBackupSwitchoverMode(uint8_t mode)
{
  //out of range ?
  if (mode > 3) mode = BSM_LEVEL;

  //Read EEPROM Backup Register (0x37)
  uint8_t reg = readConfigEEPROM_RAMmirror(EEPROM_Backup_Register);
  //register availabe ?
  if (reg == 0xFF) return false;

  //Ensure Fast Edge Detection FEDE Bit is set to 1
  reg |= 1 << EEPROMBackup_FEDE;
  //Set BSM Bits (Backup Switchover Mode)
  reg &= EEPROMBackup_BSM_CLEAR;		//Clear BSM Bits of EEPROM Backup Register
  reg |= mode << EEPROMBackup_BSM_SHIFT;	//Shift values into EEPROM Backup Register

  //To read/write from/to the EEPROM, the user has to disable the Backup Switchover function
  //by setting the BSM field to 00 or 10 (see routine in EEPROM READ/WRITE CONDITIONS)

  //Write EEPROM Backup Register
  if (writeConfigEEPROM_RAMmirror(EEPROM_Backup_Register, reg)) return true;

  return false;
}

void RV3028::enableBackupSwitchoverInterrupt(bool enable)
{
  //Read EEPROM Backup Register (0x37)
  uint8_t reg = readConfigEEPROM_RAMmirror(EEPROM_Backup_Register);
  //register availabe ?
  if (reg == 0xFF) return;

  if (enable)
    writeConfigEEPROM_RAMmirror(EEPROM_Backup_Register, reg | EEPROMBackup_BSIE);
  else
    writeConfigEEPROM_RAMmirror(EEPROM_Backup_Register, reg & ~(1 << EEPROMBackup_BSIE));
  //Serial.println( ~(1 << EEPROMBackup_BSIE), BIN);
}

//bocmar
//Changed name to readStatus()
//Returns the status byte
uint8_t RV3028::readStatus(void)
{
  return (readRegister(RV3028_STATUS));
}

//New
//clear the current interrupt flags
void RV3028::clearStatus()
{
  writeRegister(RV3028_STATUS, 0);
}

void RV3028::enableTimer(bool enable)
{
  //change only TE in CTRL1
  uint8_t reg = readRegister(RV3028_CTRL1);
  if (enable)
    writeRegister(RV3028_CTRL1, reg | 0b00000100);//set bit 3
  else
    writeRegister(RV3028_CTRL1, reg & 0b11111011);//unset bit 3
}

void RV3028::enableTimerRepeat(bool enable)
{
  //change only TRPT in CTRL1
  uint8_t reg = readRegister(RV3028_CTRL1);
  if (enable)
    writeRegister(RV3028_CTRL1, reg | 0b10000000);//set bit 7
  else
    writeRegister(RV3028_CTRL1, reg & 0b01111111);//unset bit 7
}

void RV3028::clearTimerFlag()
{
  //change only TF in STATUS
  uint8_t reg = readRegister(RV3028_STATUS);
  writeRegister(RV3028_STATUS, reg & 0b11110111);
}

//TIE Timer Interrupt Enable
void RV3028::enableTimerInterrupt(bool enable)
{
  //change only TIE in CTRL2
  uint8_t reg = readRegister(RV3028_CTRL2);
  if (enable)
    writeRegister(RV3028_CTRL2, reg | 0b00010000);//set bit 5
  else
    writeRegister(RV3028_CTRL2, reg & 0b11101111);//unset bit 5
}

void RV3028::setTimerDuration(uint16_t duration)
{
  writeRegister(RV3028_TIMERVAL_0, duration & 0xFF);  //0Ah
  writeRegister(RV3028_TIMERVAL_1, duration >> 8); //0Bh
}

uint16_t RV3028::getTimerDuration()
{
  return readRegister(RV3028_TIMERVAL_0) | (uint16_t) readRegister(RV3028_TIMERVAL_1) << 8;
}

uint16_t RV3028::getTimerCurrent()
{
  //Countdown Period = Timer Value/Timer Clock Frequency
  uint16_t current;
  //0Ch lower 8 bits;
  //0Dh upper 4 bits
  return current = readRegister(RV3028_TIMERSTAT_0) | (uint16_t) readRegister(RV3028_TIMERSTAT_1) << 8;
}

//configuration of duration
void RV3028::configureTimerFrequency(uint8_t td)
{
  //TD = 00 (4096 Hz) 244us
  //TD = 01 (64 Hz) 15ms
  //TD = 10 (1 Hz) second
  //TD = 11 (1/60 Hz) ) minute

  //change only TD in CTRL1
  uint8_t reg = readRegister(RV3028_CTRL1);

  td = td & 0b11;//only last 2 bits

  if (td == 0)
    reg =  reg & 0b11111100;
  if (td == 1)
    reg =  (reg & 0b11111101) | 0b00000001;
  if (td == 2)
    reg =  (reg & 0b11111110) | 0b00000010;
  if (td == 3)
    reg =  reg | 0b00000011;

  Serial.print(F("TD:\t"));
  Serial.println(td, BIN);
  Serial.print(F("reg&td:\t"));
  Serial.println(reg, BIN);

  writeRegister(RV3028_CTRL1, reg);

}

//PERIODIC TIME UPDATE INTERRUPT FUNCTION

void RV3028::enableUpdateInterrupt(bool enable)
{
  //change only UIE in CTRL2
  uint8_t reg = readRegister(RV3028_CTRL2);
  if (enable)
    writeRegister(RV3028_CTRL2, reg | 0b00100000);//set bit 6
  else
    writeRegister(RV3028_CTRL2, reg & 0b11011111);//unset bit 6
}

void RV3028::enableClockUpdateInterrupt(bool enable)
{
  //change only CUIE in RV3028_INT_MASK
  uint8_t reg = readRegister(RV3028_INT_MASK);
  if (enable)
    writeRegister(RV3028_INT_MASK, reg | 0b00000001);//set bit 0
  else
    writeRegister(RV3028_INT_MASK, reg & 0b11111110);//unset bit 0
}

void RV3028::clearUpdateFlag()
{
  //change only UF in STATUS
  uint8_t reg = readRegister(RV3028_STATUS);
  writeRegister(RV3028_STATUS, reg & 0b11101111);//unset bit 5
}

uint8_t RV3028::getUpdateFlag()
{
  uint8_t stat = readStatus();
  return (stat & (1 << STATUS_UF));
}

uint8_t RV3028::BCDtoDEC(uint8_t val)
{
  return ((val / 0x10) * 10) + (val % 0x10);
}

// BCDtoDEC -- convert decimal to binary-coded decimal (BCD)
uint8_t RV3028::DECtoBCD(uint8_t val)
{
  return ((val / 10) * 0x10) + (val % 10);
}

uint8_t RV3028::readRegister(uint8_t addr)
{
  _i2cPort->beginTransmission(RV3028_ADDR);
  _i2cPort->write(addr);
  _i2cPort->endTransmission();

  _i2cPort->requestFrom(RV3028_ADDR, (uint8_t)1);
  if (_i2cPort->available()) {
    uint8_t zws = _i2cPort->read();

    //clear status register when it was read
    //changed to not clear status register when it was read
    //new function clearStatus
    //bocmar
    //if (addr == RV3028_STATUS) writeRegister(addr, 0);

    return zws;
  }
  else
  {
    return (0xFF); //Error
  }
}

bool RV3028::writeRegister(uint8_t addr, uint8_t val)
{
  _i2cPort->beginTransmission(RV3028_ADDR);
  _i2cPort->write(addr);
  _i2cPort->write(val);
  if (_i2cPort->endTransmission() != 0)
    return (false); //Error: Sensor did not ack
  return (true);
}

bool RV3028::readMultipleRegisters(uint8_t addr, uint8_t * dest, uint8_t len)
{
  _i2cPort->beginTransmission(RV3028_ADDR);
  _i2cPort->write(addr);
  if (_i2cPort->endTransmission() != 0)
    return (false); //Error: Sensor did not ack

  _i2cPort->requestFrom(RV3028_ADDR, len);
  for (uint8_t i = 0; i < len; i++)
  {
    dest[i] = _i2cPort->read();
  }

  return (true);
}

bool RV3028::writeMultipleRegisters(uint8_t addr, uint8_t * values, uint8_t len)
{
  _i2cPort->beginTransmission(RV3028_ADDR);
  _i2cPort->write(addr);
  for (uint8_t i = 0; i < len; i++)
  {
    _i2cPort->write(values[i]);
  }

  if (_i2cPort->endTransmission() != 0)
    return (false); //Error: Sensor did not ack
  return (true);
}

bool RV3028::writeConfigEEPROM_RAMmirror(uint8_t eepromaddr, uint8_t val)
{
  bool success = waitforEEPROM();

  //Disable auto refresh by writing 1 to EERD control bit in CTRL1 register
  uint8_t ctrl1 = readRegister(RV3028_CTRL1);
  ctrl1 |= 1 << CTRL1_EERD;
  if (!writeRegister(RV3028_CTRL1, ctrl1)) success = false;
  //Write Configuration RAM Register
  writeRegister(eepromaddr, val);
  //Update EEPROM (All Configuration RAM -> EEPROM)
  writeRegister(RV3028_EEPROM_CMD, EEPROMCMD_First);
  writeRegister(RV3028_EEPROM_CMD, EEPROMCMD_Update);
  if (!waitforEEPROM()) success = false;
  //Reenable auto refresh by writing 0 to EERD control bit in CTRL1 register
  ctrl1 = readRegister(RV3028_CTRL1);
  if (ctrl1 == 0x00)success = false;
  ctrl1 &= ~(1 << CTRL1_EERD);
  writeRegister(RV3028_CTRL1, ctrl1);
  if (!waitforEEPROM()) success = false;

  return success;
}

uint8_t RV3028::readConfigEEPROM_RAMmirror(uint8_t eepromaddr)
{
  bool success = waitforEEPROM();

  //Disable auto refresh by writing 1 to EERD control bit in CTRL1 register
  uint8_t ctrl1 = readRegister(RV3028_CTRL1);
  ctrl1 |= 1 << CTRL1_EERD;
  if (!writeRegister(RV3028_CTRL1, ctrl1)) success = false;
  //Read EEPROM Register
  writeRegister(RV3028_EEPROM_ADDR, eepromaddr);
  writeRegister(RV3028_EEPROM_CMD, EEPROMCMD_First);
  writeRegister(RV3028_EEPROM_CMD, EEPROMCMD_ReadSingle);
  if (!waitforEEPROM()) success = false;
  uint8_t eepromdata = readRegister(RV3028_EEPROM_DATA);
  if (!waitforEEPROM()) success = false;
  //Reenable auto refresh by writing 0 to EERD control bit in CTRL1 register
  ctrl1 = readRegister(RV3028_CTRL1);
  if (ctrl1 == 0x00)success = false;
  ctrl1 &= ~(1 << CTRL1_EERD);
  writeRegister(RV3028_CTRL1, ctrl1);

  if (!success) return 0xFF;
  return eepromdata;
}

//True if success, false if timeout occured
bool RV3028::waitforEEPROM()
{
  uint32_t timeout = millis() + 500;
  while ((readRegister(RV3028_STATUS) & 1 << STATUS_EEBUSY) && millis() < timeout);

  return millis() < timeout;
}
