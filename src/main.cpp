#include <Arduino.h>

#include "local_display_MAX72X.h"
#include "local_scd30.h"
#include "mdns_support.h"
#include "web_server.h"
#include "wifi_support.h"

void setup()
{
  Serial.begin(115200);
  delay(50);

  wifi_start();
  mdns_start();
  http_start();
  MAX72X_start();
  scd30_start();
}

void loop()
{
  wifi_tick();
  mdns_tick();

  if (scd30_is_data_ready())
  {
    float co2 = -1.0f;
    float temperature = -1.0f;
    float humidity = -1.0f;

    scd30_get_values();
    scd30_read_values(co2, temperature, humidity);
    Serial.printf("co2: %f, temperature_raw: %f, humidity: %f\n", co2, temperature, humidity);
    MAX72X_print(co2, temperature, humidity);
  }

  MAX72X_tick();
  http_tick();
}
