#include "local_display_MAX72X.h"
#include <MD_MAX72XX.h>
#include <MD_Parola.h>

#define CH_SP1 '\x10' // 1 blank column

static MD_Parola g_parola(MD_MAX72XX::FC16_HW, D0, 4);

static const uint8_t SP1[] = {1, 0x00};
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

void MAX72X_print(float co2, float temp, float humidity)
{
  static char g_digits[12];

  // snprintf(g_digits, sizeof(g_digits), "%04.0f%02.0f%02.0f", co2, temp, humidity);
  snprintf(g_digits, sizeof(g_digits), "%04.0f%c%04.1f", co2, CH_SP1, temp);
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
  g_parola.setIntensity(0); // 0..15
  g_parola.setInvert(false);
  g_parola.setCharSpacing(1);
  g_parola.displayClear();
  g_parola.setFont(fontDigits3x8);

  g_parola.addChar(CH_SP1, (uint8_t*)SP1);

  MAX72X_print(-1.f, -1.f, -1.f);
}
