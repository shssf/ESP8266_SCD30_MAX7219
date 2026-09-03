#include "local_display_MAX72X.h"
#include <MD_MAX72XX.h>
#include <MD_Parola.h>
#include <math.h>

// The module chain is addressed from physical right to left. Keeping the
// zones on module boundaries gives each half of the display exactly 16 pixels.
static constexpr uint8_t ZONE_METRIC = 0; // physical right half, modules 0..1
static constexpr uint8_t ZONE_CO2 = 1;    // physical left half, modules 2..3

static constexpr uint16_t METRIC_SCROLL_STEP_MS = 125; // 16 columns ~= 2 s

#define CH_SP1          '\x10' // 1 blank column in the metric zone
#define CH_UNIT_DEGC    '\x11' // 1 blank column + lower-positioned C glyph
#define CH_UNIT_PERCENT '\x12' // 1 blank column + compact percent glyph

static MD_Parola g_parola(MD_MAX72XX::FC16_HW, D0, 4);

static char g_co2_text[8];
static char g_metric_text[8];
// It starts as true so the first actual SCD30 measurement switches to
// temperature. The startup placeholder does not affect this selector.
static bool g_show_humidity = true;

static const uint8_t SP1[] = {1, 0x00};

// Both unit glyphs have width 4. Their first blank column separates the
// value from the unit. The leading CH_SP1 in every metric string becomes the
// second blank column between the CO2 and metric halves of the display.
//
// bit 7 is the bottom LED and bit 0 is the top LED.
static const uint8_t UNIT_DEGC[] = {4, 0b00000000, 0b11110000, 0b10010011, 0b10010011};

static const uint8_t UNIT_PERCENT[] = {4, 0b00000000, 0b01011000, 0b11111000, 0b11010000};

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

static void format_co2(float co2)
{
  if (isfinite(co2) && co2 >= 0.0f && co2 < 9999.5f)
  {
    const uint16_t co2_value = (uint16_t)(co2 + 0.5f);
    snprintf(g_co2_text, sizeof(g_co2_text), "%04u", co2_value);
  }
  else
  {
    // Four normal-width dashes are easier to recognize than a compressed
    // error indicator and retain the 15-column layout of the CO2 zone.
    snprintf(g_co2_text, sizeof(g_co2_text), "----");
  }
}

// Build a 16-column value:
//   <center gap> 2 <gap> 3 . 4 <unit>
// The decimal point separates the last two digits; the unit glyph begins with
// its own blank column. The leading gap makes a two-column separator between
// the CO2 and metric halves of the display.
static void format_metric_4_chars(const char* value, char unit)
{
  g_metric_text[0] = CH_SP1;
  g_metric_text[1] = value[0];
  g_metric_text[2] = CH_SP1;
  g_metric_text[3] = value[1];
  g_metric_text[4] = value[2];
  g_metric_text[5] = value[3];
  g_metric_text[6] = unit;
  g_metric_text[7] = '\0';
}

static void format_metric_error(char unit)
{
  format_metric_4_chars("--.-", unit);
}

static void format_temperature(float temperature)
{
  if (!isfinite(temperature) || temperature < -9.9f || temperature > 99.9f)
  {
    format_metric_error(CH_UNIT_DEGC);
    return;
  }

  char value[8];
  snprintf(value, sizeof(value), "%04.1f", temperature);
  format_metric_4_chars(value, CH_UNIT_DEGC);
}

static void format_humidity(float humidity)
{
  if (!isfinite(humidity) || humidity < 0.0f || humidity > 100.0f)
  {
    format_metric_error(CH_UNIT_PERCENT);
    return;
  }

  // 100.0 would need five decimal characters. At the upper end of the
  // physical humidity range, show an unambiguous integer 100% instead.
  if (humidity >= 99.95f)
  {
    g_metric_text[0] = CH_SP1;
    g_metric_text[1] = '1';
    g_metric_text[2] = CH_SP1;
    g_metric_text[3] = '0';
    g_metric_text[4] = CH_SP1;
    g_metric_text[5] = '0';
    g_metric_text[6] = CH_UNIT_PERCENT;
    g_metric_text[7] = '\0';
    return;
  }

  char value[8];
  snprintf(value, sizeof(value), "%04.1f", humidity);
  format_metric_4_chars(value, CH_UNIT_PERCENT);
}

static void display_co2()
{
  g_parola.displayZoneText(ZONE_CO2, g_co2_text, PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
}

static void display_metric_static()
{
  g_parola.displayZoneText(ZONE_METRIC, g_metric_text, PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
}

static void scroll_metric_in()
{
  // PA_SCROLL_LEFT keeps the already-rendered metric in the zone buffer.
  // Therefore the old page moves left while the next page enters from the
  // right. A 16-column message at 125 ms per column takes about two seconds.
  g_parola.displayZoneText(ZONE_METRIC, g_metric_text, PA_LEFT, METRIC_SCROLL_STEP_MS, 0, PA_SCROLL_LEFT, PA_NO_EFFECT);
}

void MAX72X_print(float co2, float temperature, float humidity)
{
  format_co2(co2);
  display_co2();

  g_show_humidity = !g_show_humidity;
  // On an SCD30 read error all values are set to -1. Keep that from looking
  // like a real -1.0 C measurement in the metric area.
  if (!isfinite(co2) || co2 < 0.0f)
  {
    format_metric_error(g_show_humidity ? CH_UNIT_PERCENT : CH_UNIT_DEGC);
  }
  else if (g_show_humidity)
  {
    format_humidity(humidity);
  }
  else
  {
    format_temperature(temperature);
  }

  // This function is called only when SCD30 has a new measurement, so it is
  // also the only place that starts a temperature/humidity transition.
  scroll_metric_in();
}

bool MAX72X_tick()
{
  return g_parola.displayAnimate();
}

void MAX72X_start()
{
  g_show_humidity = true;
  g_parola.begin(2);
  // MD_MAX72XX numbers modules from the physical right end of the chain.
  g_parola.setZone(ZONE_METRIC, 0, 1);
  g_parola.setZone(ZONE_CO2, 2, 3);

  g_parola.setIntensity(1); // 0..15; intentionally unchanged
  g_parola.setInvert(false);
  g_parola.displayClear();

  g_parola.setFont(ZONE_METRIC, fontDigits3x8);
  g_parola.setFont(ZONE_CO2, fontDigits3x8);
  g_parola.setCharSpacing(ZONE_METRIC, 0);
  g_parola.setCharSpacing(ZONE_CO2, 1);

  g_parola.addChar(CH_SP1, SP1);
  g_parola.addChar(CH_UNIT_DEGC, UNIT_DEGC);
  g_parola.addChar(CH_UNIT_PERCENT, UNIT_PERCENT);

  // Show a static placeholder without changing the first real page: the
  // first SCD30 measurement will show temperature, then humidity, and so on.
  format_co2(-1.0f);
  format_metric_error(CH_UNIT_DEGC);
  display_co2();
  display_metric_static();
}
