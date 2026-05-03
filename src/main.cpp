#include <Arduino.h>

#include "local_display_MAX72X.h"
#include "local_diag.h"
#include "local_scd30.h"
#include "mdns_support.h"
#include "web_server.h"
#include "web_ui_pages.h"
#include "wifi_support.h"

static const uint32_t NETWORK_RESTART_PAUSE_MS = 500UL;

void network_start()
{
  wifi_start();
  mdns_start();
  http_start();
}

void network_stop()
{
  http_stop();
  mdns_stop();
  wifi_stop();
  ui_scd30_api_heartbeat_stop();
}

void network_restart()
{
  Diag.printf("Network watchcat:...");
  network_stop();
  Diag.printf(" network stoped...");
  delay(NETWORK_RESTART_PAUSE_MS);
  network_start();
  Diag.printf(" and started.\n");
}

void network_watchcat_tick()
{
  if (ui_scd30_api_alive()) // treat network stack as alive if someone can request SCD30 data via API
  {
    return;
  }

  network_restart();
}

void setup()
{
  Diag.begin(115200);
  delay(50);
  network_start();
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
    Diag.printf("co2: %f, temperature_raw: %f, humidity: %f\n", co2, temperature, humidity);
    MAX72X_print(co2, temperature, humidity);
  }

  MAX72X_tick();
  http_tick();
  network_watchcat_tick();
}
