#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

#define BUTTON_HOUR 8
#define BUTTON_MINUTE 7
#define DATA 2                    // digital 2 to pin 14 on the 74HC595
#define LATCH 3                   // digital 3 to pin 12 on the 74HC595
#define CLOCK 4                   // digital 4 to pin 11 on the 74HC595

// Variables will change:
int buttonStateHOUR = 0;          // current state of the button
int buttonStateMINUTE = 0;        // current state of the button
int lastButtonStateHOUR = 0;      // previous state of the button
int lastButtonStateMINUTE = 0;    // previous state of the button

byte registerByte1 = 0;           // 0 ES IST | 1 FUNF  | 2 ACHT | 3 VIERTEL | 4 DREI  | 5 ZEHN | 6 UND  | 7 ZWANZIG
byte registerByte2 = 0;           // 0 VOR    | 1 NACH  | 2 HALB | 3 FUNF    | 4 EIN   | 5 NEUN | 6 ZWEI | 7 DREI
byte registerByte3 = 0;           // 0 VIER   | 1 SECHS | 2 ACHT | 3 SIEBEN  | 4 ZWOLF | 5 ZEHN | 6 ELF  | 7 UHR

tmElements_t tm;

void setup()
{
  // ***********************************
  Serial.begin(9600);
  while (!Serial) ; // wait for serial
  delay(200);
  Serial.println("DS1307RTC Read Test");
  Serial.println("-------------------");
  // ***********************************
  
  // Buttons
  pinMode(BUTTON_HOUR, INPUT);
  pinMode(BUTTON_MINUTE, INPUT);
  
  // Shift Register
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);
}

void loop()
{
  // read the pushbutton input pin:
  buttonStateHOUR = digitalRead(BUTTON_HOUR);
  buttonStateMINUTE  = digitalRead(BUTTON_MINUTE);

  // Detect HOUR Button
  if (buttonStateHOUR != lastButtonStateHOUR) {
    if (buttonStateHOUR == HIGH) {
      addTime(1,0);
    }
    lastButtonStateHOUR = buttonStateHOUR;
  }
  // Detect MINUTE Button
  if (buttonStateMINUTE != lastButtonStateMINUTE) {
    if (buttonStateMINUTE == HIGH) {
      addTime(0,1);
    }
    lastButtonStateMINUTE = buttonStateMINUTE;
  }

  // Generate time and shiftOut
  timeToBytes();
  updateShiftRegisters();
  printTime();
}


void timeToBytes()
{
  if (RTC.read(tm)) {
    int deutschHour = shortHour(tm.Hour);
    int halb = 0;
    registerByte1 = 0;
    registerByte2 = 0;
    registerByte3 = 0;
    bitSet(registerByte1,0);    // "ES IST"
    
    if(tm.Minute >= 3 && tm.Minute <= 4) {
      bitSet(registerByte1,4);    // "DREI"
      bitSet(registerByte2,1);    // "NACH"
    } else if (tm.Minute >= 5 && tm.Minute <= 7) {
      bitSet(registerByte1,1);    // "FUNF"
      bitSet(registerByte2,1);    // "NACH"
    } else if (tm.Minute >= 8 && tm.Minute <= 9) {
      bitSet(registerByte1,2);    // "ACHT"
      bitSet(registerByte2,1);    // "NACH"
    } else if (tm.Minute >= 10 && tm.Minute <= 12) {
      bitSet(registerByte1,5);    // "ZEHN"
      bitSet(registerByte2,1);    // "NACH"
    } else if (tm.Minute >= 13 && tm.Minute <= 14) {
      bitSet(registerByte1,4);    // "DREI"
      bitSet(registerByte1,5);    // "ZEHN"
      bitSet(registerByte2,1);    // "NACH"
    } else if (tm.Minute >= 15 && tm.Minute <= 17) {
      bitSet(registerByte1,3);    // "VIERTEL"
      bitSet(registerByte2,1);    // "NACH"
    } else if (tm.Minute >= 18 && tm.Minute <= 19) {
      bitSet(registerByte1,2);    // "ACHT"
      bitSet(registerByte1,5);    // "ZEHN"
      bitSet(registerByte2,1);    // "NACH"
    } else if (tm.Minute >= 20 && tm.Minute <= 21)  {
      bitSet(registerByte1,7);    // "ZWANZIG"
      bitSet(registerByte2,1);    // "NACH"
    } else if (tm.Minute >= 22 && tm.Minute <= 24) {
      bitSet(registerByte1,2);    // "ACHT"
      bitSet(registerByte2,0);    // "VOR"
      bitSet(registerByte2,2);    // "HALB"
      halb = 1;
    } else if (tm.Minute >= 25 && tm.Minute <= 26) {
      bitSet(registerByte1,1);    // "FUNF"
      bitSet(registerByte2,0);    // "VOR"
      bitSet(registerByte2,2);    // "HALB"
      halb = 1;
    } else if (tm.Minute >= 27 && tm.Minute <= 29) {
      bitSet(registerByte1,4);    // "DREI"
      bitSet(registerByte2,0);    // "VOR"
      bitSet(registerByte2,2);    // "HALB"
      halb = 1;
    } else if (tm.Minute >= 30 && tm.Minute <= 32) {
      bitSet(registerByte2,2);    // "HALB"
      halb = 1;
    } else if (tm.Minute >= 33 && tm.Minute <= 34) {
      bitSet(registerByte1,4);    // "DREI"
      bitSet(registerByte2,1);    // "NACH"
      bitSet(registerByte2,2);    // "HALB"
      halb = 1;
    } else if (tm.Minute >= 35 && tm.Minute <= 37) {
      bitSet(registerByte1,1);    // "FUNF"
      bitSet(registerByte2,1);    // "NACH"
      bitSet(registerByte2,2);    // "HALB"
      halb = 1;
    } else if (tm.Minute >= 38 && tm.Minute <= 39) {
      bitSet(registerByte1,2);    // "ACHT"
      bitSet(registerByte2,1);    // "NACH"
      bitSet(registerByte2,2);    // "HALB"
      halb = 1;
    } else if (tm.Minute >= 40 && tm.Minute <= 41) {
      bitSet(registerByte1,7);    // "ZWANZIG"
      bitSet(registerByte2,0);    // "VOR"
      halb = 1;
    } else if (tm.Minute >= 42 && tm.Minute <= 44) {
      bitSet(registerByte1,2);    // "ACHT"
      bitSet(registerByte1,5);    // "ZEHN"
      bitSet(registerByte2,0);    // "VOR"
      halb = 1;
    } else if (tm.Minute >= 45 && tm.Minute <= 46) {
      bitSet(registerByte1,3);    // "VIERTEL"
      bitSet(registerByte2,0);    // "VOR"
      halb = 1;
    } else if (tm.Minute >= 47 && tm.Minute <= 49) {
      bitSet(registerByte1,4);    // "DREI"
      bitSet(registerByte1,5);    // "ZEHN"
      bitSet(registerByte2,0);    // "VOR"
      halb = 1;
    } else if (tm.Minute >= 50 && tm.Minute <= 51) {
      bitSet(registerByte1,5);    // "ZEHN"
      bitSet(registerByte2,0);    // "VOR"
      halb = 1;
    } else if (tm.Minute >= 52 && tm.Minute <= 54) {
      bitSet(registerByte1,2);    // "ACHT"
      bitSet(registerByte2,0);    // "VOR"
      halb = 1;
    } else if (tm.Minute >= 55 && tm.Minute <= 56) {
      bitSet(registerByte1,1);    // "FUNF"
      bitSet(registerByte2,0);    // "VOR"
      halb = 1;
    } else if (tm.Minute >= 57 && tm.Minute <= 59) {
      bitSet(registerByte1,4);    // "DREI"
      bitSet(registerByte2,0);    // "VOR"
      halb = 1;
    }

    deutschHour = shortHour( deutschHour + halb);
    if ( deutschHour == 1 ) {
      bitSet(registerByte2,4);    // "EIN"
    } else if ( deutschHour == 2 ) {
      bitSet(registerByte2,6);    // "ZWEI"
    } else if ( deutschHour == 3 ) {
      bitSet(registerByte2,7);    // "DREI"
    } else if ( deutschHour == 4 ) {
      bitSet(registerByte3,0);    // "VIER"
    } else if ( deutschHour == 5 ) {
      bitSet(registerByte2,3);    // "FUNF"
    } else if ( deutschHour == 6 ) {
      bitSet(registerByte3,1);    // "SECHS"
    } else if ( deutschHour == 7 ) {
      bitSet(registerByte3,3);    // "SEIBEN"
    } else if ( deutschHour == 8 ) {
      bitSet(registerByte3,2);    // "ACHT"
    } else if ( deutschHour == 9 ) {
      bitSet(registerByte2,5);    // "NEUN"
    } else if ( deutschHour == 10 ) {
      bitSet(registerByte3,5);    // "ZEHN"
    } else if ( deutschHour == 11 ) {
      bitSet(registerByte3,6);    // "ELF"
    } else if ( deutschHour == 12 ) {
      bitSet(registerByte3,4);    // "ZWOLF"
    }
    bitSet(registerByte3,7);    // "UHR"
  }
}

void updateShiftRegisters()
{
   digitalWrite(LATCH, LOW);
   shiftOut(DATA, CLOCK, MSBFIRST, registerByte3);
   shiftOut(DATA, CLOCK, MSBFIRST, registerByte2);
   shiftOut(DATA, CLOCK, MSBFIRST, registerByte1);
   digitalWrite(LATCH, HIGH);
}

void addTime(int addHour, int addMinute)
{
  // get time from RTC
  if (RTC.read(tm)) {

    if (addHour > 0) {
      tm.Hour = tm.Hour + addHour;
      if (tm.Hour > 12) {
        tm.Hour = tm.Hour - 12;
      }
    }
    
    if (addMinute > 0) {
      tm.Minute = tm.Minute + addMinute;
      if (tm.Minute > 59) {
        tm.Minute = tm.Minute - 60;
        addTime(1,0);
      }
    }  

    tm.Second = 0;
    RTC.write(tm); 
  }
}

int shortHour(int tempHour)
{
  tempHour = tempHour % 12;
  return tempHour == 0 ? 12 : tempHour;
}









void printTime() {

  if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
    Serial.print("registerByte3 = ");
    Serial.print(registerByte3, BIN);
    Serial.println();
    Serial.print("registerByte2 = ");
    Serial.print(registerByte2, BIN);
    Serial.println();
    Serial.print("registerByte1 = ");
    Serial.print(registerByte1, BIN);
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(1000);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
