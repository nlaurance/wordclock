#include <Arduino.h>

// led strip
#include <FastLED.h>

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
// SDA goes to pin A4
// SCL goes to pin A5
#include "RTClib.h"

// humidity / thermo
#include "DHT.h"

const int DisplayPin = 10;
const int PhotoSensorensorPin = A0;
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
RTC_DS1307 rtc;
int luminosity=0;
uint8_t brightness=128;


const int NUM_LEDS=144;
CRGB strip[NUM_LEDS];


int from_grid_to_leds(int line, int col) {
  if ( line % 2 == 0) {
    return NUM_COLS * line + col;
  }
  else {
    return NUM_COLS * line + (NUM_COLS - 1) - col;
  }
}


void empty_display() {
  for (int i=0; i < NUM_LEDS; i++) {
      strip[i] = CRGB::Black;
  }
}

void reset_grid(int grid[][12]) {
  for (int l=0; l < NUM_LINES; l++) {
    for (int c=0; c < NUM_COLS; c++) {
      grid[l][c]=0;
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

void display_word(const int word[] , CRGB color) {
  for (int i=word[0]; i < word[1]; i++) {
    strip[i]=color;
  }
}

void display_grid(uint8_t grid[][12], uint8_t color) {
  for (int l=0; l < NUM_LINES; l++) {
    for (int c=0; c < NUM_COLS; c++) {
      int led = from_grid_to_leds(l, c);
      if (grid[l][c]==1) {
        strip[led] = CHSV( color, 255, brightness);
      }
      else {
        strip[led] = CRGB::Black;
      }
    }
  }
  FastLED.show();
}

void display_digit(uint8_t grid[][12], int value, int line, int col) {

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
  // temperature = 24;
  uint8_t color = map(temperature, 16, 24, 160, 0);
  // uint32_t color = strip.Color(0, 0, 127);
  do {
    int digit = temperature % 10;
    display_digit(base_grid, digit, line, col);
    col -= 6;
    temperature /= 10;
  } while (temperature > 0);
  display_grid(base_grid, color);
}

void write_time(int hour, int minute, int second) {
  CRGB color = CRGB::HotPink;

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
      break;
    case 8:
      display_word(huit, color);
      display_word(heures, color);
      break;
    case 9:
      display_word(neuf, color);
      display_word(heures, color);
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
      break;
    case 22:
      display_word(dix, color);
      display_word(heures, color);
      break;
    case 23:
      display_word(onze, color);
      display_word(heures, color);
      break;
  }

  FastLED.show();
}

void display_time() {
  DateTime now = rtc.now();
  int hour = now.hour();
  int minute =  now.minute();
  int second =  now.second();
  write_time(hour, minute, second);
}

// void display_celebrations() {
//   uint32_t color = strip.Color(200, 0, 0);
//   DateTime now = rtc.now();
//   int month = now.month();
//   int day =  now.day();
//   if (day == 25 && month == 12) {
//     display_word(joyeux, color);
//     display_word(noel, color);
//   }
//   if (day == 1 && month == 1) {
//     display_word(bonne, color);
//     display_word(annee, color);
//   }
//   strip.show();
// }


void startup_test () {
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    strip[i] = CHSV( 224, 187, brightness);
    FastLED.show();
    delay(5);
  }
  fill_rainbow( &(strip[0]), NUM_LEDS, 222 /*starting hue*/);
  FastLED.show();
  delay(2000);
  empty_display();
  FastLED.show();

}

void setup () {

  dht.setup(A1);

  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

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

  FastLED.addLeds<NEOPIXEL, DisplayPin>(strip, NUM_LEDS);
  FastLED.show();

  // startup_test();
}


void loop () {

  luminosity = analogRead(PhotoSensorensorPin);
  brightness = map(luminosity, 10, 1024, 64, 255);

  empty_display();
  display_thermo();
  delay(5000);

  empty_display();
  display_time();
  delay(55000);
}
