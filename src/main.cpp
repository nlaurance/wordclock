#include <Arduino.h>

// led strip
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
// SDA goes to pin A4
// SCL goes to pin A5
#include "RTClib.h"

// humidity / thermo
#include "DHT.h"

const int SetClockPin = 5;
const int ClockUpPin = 6;
const int ClockDownPin = 7;

const int DisplayPin = 10;


const int RunClock = 0;
const int SetYear = 1;
const int SetMonth = 2;
const int SetDay = 3;
const int SetHour = 4;
const int SetMinute = 5;

//
uint8_t base_grid[12][12] = {
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
};

const uint8_t digit_zero[6][5] ={
  {0,1,1,1,0},
  {1,0,0,0,1},
  {1,0,0,0,1},
  {1,0,0,0,1},
  {1,0,0,0,1},
  {0,1,1,1,0},
};
const uint8_t digit_one [6][5]= {
  {0,0,1,0,0},
  {0,1,1,0,0},
  {0,0,1,0,0},
  {0,0,1,0,0},
  {0,0,1,0,0},
  {0,1,1,1,0},
};
const uint8_t digit_two [6][5]= {
  {0,1,1,1,0},
  {1,0,0,0,1},
  {0,0,1,0,0},
  {0,1,0,0,0},
  {1,0,0,0,0},
  {1,1,1,1,1},
};
const uint8_t digit_three [6][5]= {
  {0,1,1,1,0},
  {1,0,0,0,1},
  {0,0,1,0,0},
  {0,0,0,1,0},
  {1,0,0,0,1},
  {0,1,1,1,0},
};
const uint8_t digit_four [6][5]= {
  {0,0,0,1,0},
  {0,0,1,1,0},
  {0,1,0,1,0},
  {1,0,0,1,0},
  {1,1,1,1,1},
  {0,0,0,1,0},
};
const uint8_t digit_five [6][5]= {
  {1,1,1,1,1},
  {1,0,0,0,0},
  {1,0,0,0,0},
  {0,1,1,1,0},
  {0,0,0,0,1},
  {1,1,1,1,0},
};
const uint8_t digit_six [6][5]= {
  {0,0,1,1,1},
  {0,1,0,0,0},
  {1,0,0,0,0},
  {1,1,1,1,0},
  {1,0,0,0,1},
  {0,1,1,1,0},
};
const uint8_t digit_seven [6][5]= {
  {1,1,1,1,1},
  {1,0,0,0,1},
  {0,0,0,1,0},
  {0,0,1,0,0},
  {0,0,1,0,0},
  {0,0,1,0,0},
};
const uint8_t digit_eight [6][5]= {
  {0,1,1,1,0},
  {1,0,0,0,1},
  {0,1,1,1,0},
  {0,1,0,1,0},
  {1,0,0,0,1},
  {0,1,1,1,0},
};
const uint8_t digit_nine [6][5]= {
  {0,1,1,1,0},
  {1,0,0,0,1},
  {0,1,1,1,0},
  {0,0,0,0,1},
  {0,0,0,0,1},
  {0,1,1,1,0},
};

const int NUM_COLS = 12;
const int NUM_LINES = 12;

// hours in letters, indices
const int une[2] = {13, 16};
const int deux[2] = {30, 34};
const int trois[2] = {90, 95};
const int quatre[2] = {60, 66};
const int cinq[2] = {73, 77};
const int six[2] = {20, 23};
const int sept[2] = {26, 30};
const int huit[2] = {6, 10};
const int neuf[2] = {1, 5};
const int dix[2] = {84, 87};
const int onze[2] = {16, 20};
const int midi[2] = {36, 40};
const int minuit[2] = {49, 55};

const int heure[2] = {97, 102};
const int heures[2] = {97, 103};
const int moins[2] = {109, 114};
const int le[2] = {124, 126};
const int et[2] = {105, 107};
const int quart[2] = {127, 132};
const int vingt[2] = {138, 143};
const int mcinq[2] = {133, 137};
const int mdix[2] = {120, 123};
const int demie[2] = {115, 120};

const int bonne[2] = {43, 48};
const int annee[2] = {55, 60};
const int joyeux[2] = {66, 72};
const int noel[2] = {78, 82};

DHT dht;

// variables will change:
int SetClockState = 0;
int ClockUpState = 0;
int ClockDownState = 0;

int SetYearTo = 0;
int SetMonthTo = 0;
int SetDayTo = 0;
int SetHourTo = 0;
int SetMinuteTo = 0;
bool settingsInProgress = false;

int clockMode = RunClock;

RTC_DS1307 rtc;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(144, DisplayPin, NEO_GRB + NEO_KHZ800);

int from_grid_to_leds(int line, int col) {
  if ( line % 2 == 0) {
    return NUM_COLS * line + col;
  }
  else {
    return NUM_COLS * line + (NUM_COLS - 1) - col;
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void empty_display() {
  for (int i=0; i < NUM_LINES*NUM_COLS; i++) {
      strip.setPixelColor(i, 0);
  }

}

void reset_grid(int grid[][12]) {
  for (int l=0; l < NUM_LINES; l++) {
    for (int c=0; c < NUM_COLS; c++) {
      grid[l][c]=0;
    }
  }
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void fill_grid(int wait) {
  for (int l=0; l < NUM_LINES; l++) {
    for (int c=0; c < NUM_COLS; c++) {
      int led = from_grid_to_leds(l, c);
      strip.setPixelColor(led, Wheel( 2*(l+c) % 255));
      strip.show();
      delay(wait);
    }
  }
}

void paste_sprite(uint8_t grid[][12], const uint8_t sprite[][5], int line, int col) {
  for (int l=0; l < 6; l++) { // num lines in sprite
    for (int c=0; c < 5; c++) { // num cols in sprite
      int shift_line = l + line;
      int shift_col = c + col;

      if (shift_line >=0 && shift_line<NUM_LINES
        && shift_col >=0 && shift_col<NUM_COLS) {
        grid[l+line][c+col] = sprite[l][c];
        // rotate
        // grid[c+col][12-l-line] = sprite[l][c];
      }
    }
  }
}

void display_word(const int word[] , uint32_t color) {
  for (int i=word[0]; i < word[1]; i++) {
    strip.setPixelColor(i, color);
  }
}

void display_grid(uint8_t grid[][12], uint32_t color) {
  for (int l=0; l < NUM_LINES; l++) {
    for (int c=0; c < NUM_COLS; c++) {
      int led = from_grid_to_leds(l, c);
      if (grid[l][c]==1) {
        strip.setPixelColor(led, color); // blue
      }
      else {
        strip.setPixelColor(led, 0);
      }
    }
  }
  strip.show();
}

void display_digit(uint8_t grid[][12], int value, int line, int col) {
  // if (value == 0) {
  //   paste_sprite(grid, digit_zero, line, col);}
  // else if (value == 1) {
  //   paste_sprite(grid, digit_one, line, col);}
  // else if (value == 2) {
  //   paste_sprite(grid, digit_two, line, col);}
  // else if (value == 3) {
  //   paste_sprite(grid, digit_three, line, col);}
  // else if (value == 4) {
  //   paste_sprite(grid, digit_four, line, col);}
  // else if (value == 5) {
  //   paste_sprite(grid, digit_five, line, col);}
  // else if (value == 6) {
  //   paste_sprite(grid, digit_six, line, col);}
  // else if (value == 7) {
  //   paste_sprite(grid, digit_seven, line, col);}
  // else if (value == 8) {
  //   paste_sprite(grid, digit_eight, line, col);}
  // else if (value == 9) {
  //   paste_sprite(grid, digit_nine, line, col);}


  switch(value) {
    case 0:
      paste_sprite(grid, digit_zero, line, col);
      break;
    case 1:
      paste_sprite(grid, digit_one, line, col);
      break;
    case 2:
      paste_sprite(grid, digit_two, line, col);
      break;
    case 3:
      paste_sprite(grid, digit_three, line, col);
      break;
    case 4:
      paste_sprite(grid, digit_four, line, col);
      break;
    case 5:
      paste_sprite(grid, digit_five, line, col);
      break;
    case 6:
      paste_sprite(grid, digit_six, line, col);
      break;
    case 7:
      paste_sprite(grid, digit_seven, line, col);
      break;
    case 8:
      paste_sprite(grid, digit_eight, line, col);
      break;
    case 9:
      paste_sprite(grid, digit_nine, line, col);
      break;
    }

}

void display_thermo() {
  int temperature = static_cast<byte>(dht.getTemperature());
  // int temperature = 19;
  int line = 3;
  int col = 6;
  strip.setBrightness(255);
  // temperature = 24;
  uint32_t color = Wheel(map(temperature, 16, 24, 128, 0));
  // uint32_t color = strip.Color(0, 0, 127);
  do {
    int digit = temperature % 10;
    display_digit(base_grid, digit, line, col);
    col -= 6;
    temperature /= 10;
    Serial.println(digit);
  } while (temperature > 0);
  display_grid(base_grid, color);
}

void write_time(int hour, int minute, int second) {
  uint32_t color = strip.Color(0, 0, 127);
  int seconds_past_hour = minute * 60 + second;
  bool shift_hour = false;

  if (seconds_past_hour < 150) {}
  else if (seconds_past_hour < 450) {
      display_word(mcinq, color);}
  else if (seconds_past_hour < 750) {
      display_word(mdix, color);}
  else if (seconds_past_hour < 1050) {
    display_word(et, color);
    display_word(quart, color);
    }
  else if (seconds_past_hour < 1350) {
    display_word(vingt, color);
    }
  else if (seconds_past_hour < 1650) {
    display_word(vingt, color);
    display_word(mcinq, color);
    }
  else if (seconds_past_hour < 1950) {
    display_word(et, color);
    display_word(demie, color);
    }
  else if (seconds_past_hour < 2250) {
    display_word(moins, color);
    display_word(vingt, color);
    display_word(mcinq, color);
    shift_hour = true;
    }
  else if (seconds_past_hour < 2550) {
    display_word(moins, color);
    display_word(vingt, color);
    shift_hour = true;
    }
  else if (seconds_past_hour < 2850) {
    display_word(moins, color);
    display_word(le, color);
    display_word(quart, color);
    shift_hour = true;
    }
  else if (seconds_past_hour < 3150) {
    display_word(moins, color);
    display_word(mdix, color);
    shift_hour = true;
    }
  else if (seconds_past_hour < 3450) {
    display_word(moins, color);
    display_word(mcinq, color);
    shift_hour = true;
    }
    else {
      shift_hour = true;
    }

  if (shift_hour) {
    hour = (hour+1) % 24;
  }

  switch (hour) {
    case 0:
      display_word(minuit, color);
      break;
    case 1:
      display_word(une, color);
      display_word(heure, color);
      break;
    case 2:
      display_word(deux, color);
      display_word(heures, color);
      break;
    case 3:
      display_word(trois, color);
      display_word(heures, color);
      break;
    case 4:
      display_word(quatre, color);
      display_word(heures, color);
      break;
    case 5:
      display_word(cinq, color);
      display_word(heures, color);
      break;
    case 6:
      display_word(six, color);
      display_word(heures, color);
      break;
    case 7:
      display_word(sept, color);
      display_word(heures, color);
      strip.setBrightness(150);
      break;
    case 8:
      display_word(huit, color);
      display_word(heures, color);
      break;
    case 9:
      display_word(neuf, color);
      display_word(heures, color);
      strip.setBrightness(240);
      break;
    case 10:
      display_word(dix, color);
      display_word(heures, color);
      break;
    case 11:
      display_word(onze, color);
      display_word(heures, color);
      break;
    case 12:
      display_word(midi, color);
      break;
    case 13:
      display_word(une, color);
      display_word(heure, color);
      break;
    case 14:
      display_word(deux, color);
      display_word(heures, color);
      break;
    case 15:
      display_word(trois, color);
      display_word(heures, color);
      break;
    case 16:
      display_word(quatre, color);
      display_word(heures, color);
      break;
    case 17:
      display_word(cinq, color);
      display_word(heures, color);
      break;
    case 18:
      display_word(six, color);
      display_word(heures, color);
      strip.setBrightness(230);
      break;
    case 19:
      display_word(sept, color);
      display_word(heures, color);
      break;
    case 20:
      display_word(huit, color);
      display_word(heures, color);
      break;
    case 21:
      display_word(neuf, color);
      display_word(heures, color);
      strip.setBrightness(150);
      break;
    case 22:
      display_word(dix, color);
      display_word(heures, color);
      break;
    case 23:
      display_word(onze, color);
      display_word(heures, color);
      strip.setBrightness(64);
      break;
  }

  strip.show();
}

void display_time() {
  DateTime now = rtc.now();
  int hour = now.hour();
  int minute =  now.minute();
  int second =  now.second();
  write_time(hour, minute, second);
}

void display_celebrations() {
  uint32_t color = strip.Color(200, 0, 0);
  DateTime now = rtc.now();
  int month = now.month();
  int day =  now.day();
  if (day == 25 && month == 12) {
    display_word(joyeux, color);
    display_word(noel, color);
  }
  if (day == 1 && month == 1) {
    display_word(bonne, color);
    display_word(annee, color);
  }
  strip.show();
}

void startup_test () {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
    strip.show();
    delay(5);
  }
  empty_display();
  strip.show();

  empty_display();
  strip.show();


}

void setup () {
  // while (!Serial); // for Leonardo/Micro/Zero
  pinMode(SetClockPin, INPUT);
  pinMode(ClockUpPin, INPUT);
  pinMode(ClockDownPin, INPUT);

  dht.setup(A1);

  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  strip.begin();
  strip.setBrightness(64);
  strip.show(); // Initialize all pixels to 'off'
  Serial.print("founds leds: ");
  Serial.println(strip.numPixels());
  startup_test();

}


void loop () {

  // read the state of the pushbutton value:
    SetClockState = digitalRead(SetClockPin);
    if (SetClockState == HIGH) {
      clockMode++;
      settingsInProgress = false;
      if (clockMode > SetMinute) {
        clockMode = RunClock;
        rtc.adjust(DateTime(SetYearTo, SetMonthTo, SetDayTo,
          SetHourTo, SetMinuteTo, 0));
        }
    }

    switch (clockMode) {
      case RunClock:
        // printCurrentTime();

        break;
      case SetYear:
        if (!settingsInProgress) {
          DateTime now = rtc.now();
          SetYearTo = now.year();
          settingsInProgress = true;
        }

        ClockUpState = digitalRead(ClockUpPin);
        ClockDownState = digitalRead(ClockDownPin);

        if (ClockUpState == HIGH) {
          SetYearTo++; }
        if (ClockDownState == HIGH) {
          SetYearTo--; }
        Serial.print("Year setting: ");
        Serial.print(SetYearTo, DEC);
        Serial.println("");
        break;
      case SetMonth:
        if (!settingsInProgress) {
          DateTime now = rtc.now();
          SetMonthTo = now.month();
          settingsInProgress = true;
        }

        ClockUpState = digitalRead(ClockUpPin);
        ClockDownState = digitalRead(ClockDownPin);

        if (ClockUpState == HIGH) {
          SetMonthTo++;
          if (SetMonthTo > 12) {
            SetMonthTo = 1;
          }
        }

        if (ClockDownState == HIGH) {
          SetMonthTo--;
          if (SetMonthTo < 1) {
            SetMonthTo = 12;
          }
        }

        Serial.print("Month setting: ");
        Serial.print(SetMonthTo, DEC);
        Serial.println("");
        break;
      case SetDay:
        if (!settingsInProgress) {
          DateTime now = rtc.now();
          SetDayTo = now.day();
          settingsInProgress = true;
        }

        ClockUpState = digitalRead(ClockUpPin);
        ClockDownState = digitalRead(ClockDownPin);

        if (ClockUpState == HIGH) {
          SetDayTo++;
          if (SetDayTo > 31) {
            SetDayTo = 1;
          }
        }

        if (ClockDownState == HIGH) {
          SetDayTo--;
          if (SetDayTo < 1 ) {
            SetDayTo = 31;
          }
        }

        Serial.print("Day setting: ");
        Serial.print(SetDayTo, DEC);
        Serial.println("");
        break;
      case SetHour:
        if (!settingsInProgress) {
          DateTime now = rtc.now();
          SetHourTo = now.hour();
          settingsInProgress = true;
        }

        ClockUpState = digitalRead(ClockUpPin);
        ClockDownState = digitalRead(ClockDownPin);

        if (ClockUpState == HIGH) {
          SetHourTo++;
          if (SetHourTo > 23) {
            SetHourTo = 0;
          }
        }

        if (ClockDownState == HIGH) {
          SetHourTo--;
          if (SetHourTo < 0) {
            SetHourTo = 23;
          }
        }

        Serial.print("Hour setting: ");
        Serial.print(SetHourTo, DEC);
        Serial.println("");
        break;
      case SetMinute:
        if (!settingsInProgress) {
          DateTime now = rtc.now();
          SetMinuteTo = now.minute();
          settingsInProgress = true;
        }

        ClockUpState = digitalRead(ClockUpPin);
        ClockDownState = digitalRead(ClockDownPin);

        if (ClockUpState == HIGH) {
          SetMinuteTo++;
          if (SetMinuteTo > 59) {
            SetMinuteTo = 0;
          }
        }
        if (ClockDownState == HIGH) {
          SetMinuteTo--;
          if (SetMinuteTo < 0) {
            SetMinuteTo = 59;
          }
        }
        Serial.print("Minute setting: ");
        Serial.print(SetMinuteTo, DEC);
        Serial.println("");
        break;
  }

    empty_display();
    display_thermo();
    delay(5000);

    empty_display();
    display_time();
    delay(2000);

}
