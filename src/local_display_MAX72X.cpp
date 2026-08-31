#include "local_display_MAX72X.h"
#include <MD_MAX72XX.h>
#include <MD_Parola.h>

#define CH_SP1          '\x10' // 1 blank column
#define CH_CO2_K1       '\x11' // CO2 thousands: 1
#define CH_CO2_K2       '\x12' // CO2 thousands: 2
#define CH_CO2_K3       '\x13' // CO2 thousands: 3
#define CH_CO2_K4       '\x14' // CO2 thousands: 4
#define CH_CO2_K5       '\x15' // CO2 thousands: 5
#define CH_CO2_OVERFLOW '\x16'
#define CH_SP4          '\x17' // 4 blank columns

static MD_Parola g_parola(MD_MAX72XX::FC16_HW, D0, 4);

static const uint8_t SP1[] = {1, 0x00};
static const uint8_t SP4[] = {4, 0x00, 0x00, 0x00, 0x00};

// Compact one-column CO2 thousands indicator. A value of 1 is drawn as a
// regular narrow "1"; 2..5 are encoded by that many pixels from the bottom.
// Values above 5 use two vertically separated dots to indicate overflow.
static const uint8_t CO2_K1[] = {1, 0b11111111};
static const uint8_t CO2_K2[] = {1, 0b11000000};
static const uint8_t CO2_K3[] = {1, 0b11100000};
static const uint8_t CO2_K4[] = {1, 0b11110000};
static const uint8_t CO2_K5[] = {1, 0b11111000};
static const uint8_t CO2_OVERFLOW[] = {1, 0b00100100};

static const char CO2_THOUSANDS_CHARS[] = {
    CH_SP1, CH_CO2_K1, CH_CO2_K2, CH_CO2_K3, CH_CO2_K4, CH_CO2_K5, CH_CO2_OVERFLOW, CH_CO2_OVERFLOW, CH_CO2_OVERFLOW, CH_CO2_OVERFLOW};

static const MD_MAX72XX::fontType_t fontDigits3x8[] PROGMEM = {
    'F',
    2,
    0x00,
    '-',
    0x00,
    '9',
    8, // 'F', ver=2, first='0', last='9', height=8

    // Each character entry: <width=3>, <column0>, <column1>, <column2>
    // In each column: bit7 = bottom pixel, bit0 = top pixel.

    // '-' (ASCII 45)
    3,
    0b00010000,
    0b00010000,
    0b00010000,

    // '.' (ASCII 46)
    1,
    0b10000000,

    // '/' (ASCII 47)
    3,
    0b11000000,
    0b00010000,
    0b00000011,

    // '0'
    3,
    0b11111111,
    0b10000001,
    0b11111111,

    // '1'
    3,
    0b00000000,
    0b11111111,
    0b00000000,

    // '2'
    3,
    0b11100001,
    0b10010001,
    0b10001111,

    // '3'
    3,
    0b10010001,
    0b10010001,
    0b11111111,

    // '4'
    3,
    0b00011111,
    0b00010000,
    0b11111111,

    // '5'
    3,
    0b10011111,
    0b10010001,
    0b11110001,

    // '6'
    3,
    0b11111111,
    0b10010001,
    0b11110001,

    // '7'
    3,
    0b11100001,
    0b00010001,
    0b00001111,

    // '8'
    3,
    0b11111111,
    0b10010001,
    0b11111111,

    // '9'
    3,
    0b10011111,
    0b10010001,
    0b11111111,
};

// Format an invalid 10-column display block as right-aligned "-1".
static void overflow_print(char* buf)
{
  buf[0] = CH_SP4;
  buf[1] = '-';
  buf[2] = '1';
  buf[3] = '\0';
}

void MAX72X_print(float co2, float temp, float humidity)
{
  static char g_digits[16];
  char co2_digits[8];
  char temp_digits[8];
  char humidity_digits[8];

  // CO2 block: 0..9999 ppm, 10 columns; show -1 outside the valid range.
  if (co2 >= 0.0f && co2 <= 9999.0f)
  {
    const int co2_value = (int)(co2 + 0.5f);
    const int co2_thousands = co2_value / 1000;
    const int co2_remainder = co2_value % 1000;
    snprintf(co2_digits, sizeof(co2_digits), "%c%03d", CO2_THOUSANDS_CHARS[co2_thousands], co2_remainder);
  }
  else
  {
    overflow_print(co2_digits);
  }

  // Temperature block: -9.9..99.9 C, 10 columns; show -1 outside the valid range.
  if (temp >= -9.9f && temp <= 99.9f)
  {
    snprintf(temp_digits, sizeof(temp_digits), "%04.1f", temp);
  }
  else
  {
    overflow_print(temp_digits);
  }

  // Humidity block: 0..99.9 percent, 10 columns; show -1 outside the valid range.
  if (humidity >= 0.0f && humidity <= 99.9f)
  {
    snprintf(humidity_digits, sizeof(humidity_digits), "%04.1f", humidity);
  }
  else
  {
    overflow_print(humidity_digits);
  }

  snprintf(g_digits, sizeof(g_digits), "%s%c%s%c%s", co2_digits, CH_SP1, temp_digits, CH_SP1, humidity_digits);
  // Serial.print(g_digits);
  g_parola.displayText(g_digits, PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
  g_parola.displayReset();
}

bool MAX72X_tick()
{
  return g_parola.displayAnimate();
}

void MAX72X_start()
{
  g_parola.begin();
  g_parola.setIntensity(1); // 0..15
  g_parola.setInvert(false);
  g_parola.setCharSpacing(0);
  g_parola.displayClear();
  g_parola.setFont(fontDigits3x8);

  g_parola.addChar(CH_SP1, SP1);
  g_parola.addChar(CH_SP4, SP4);
  g_parola.addChar(CH_CO2_K1, CO2_K1);
  g_parola.addChar(CH_CO2_K2, CO2_K2);
  g_parola.addChar(CH_CO2_K3, CO2_K3);
  g_parola.addChar(CH_CO2_K4, CO2_K4);
  g_parola.addChar(CH_CO2_K5, CO2_K5);
  g_parola.addChar(CH_CO2_OVERFLOW, CO2_OVERFLOW);

  MAX72X_print(-1.f, -1.f, -1.f);
}
