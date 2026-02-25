#include <Arduino.h>

#include "web_server.h"
#include "web_ui_pages.h"

#include "local_scd30.h"

static void handleApiScd30(void)
{
  char json[128];
  float s_last_co2 = 0.0f;
  float s_last_t = 0.0f;
  float s_last_rh = 0.0f;

  scd30_read_values(s_last_co2, s_last_t, s_last_rh);
  snprintf(json, sizeof(json), "{\"co2_ppm\":%.2f,\"temperature_c\":%.2f,\"humidity_pct\":%.2f}", s_last_co2, s_last_t, s_last_rh);
  http_send(200, "application/json; charset=utf-8", json);
}

static void handleScd30Page(void)
{
  if (http_send_littlefs_file("/scd30_page.html", "text/html; charset=utf-8"))
  {
    return;
  }

  http_send(404, "text/plain; charset=utf-8", "scd30_page.html not found in LittleFS");
}

void ui_register_scd30_routes(void)
{
  http_route_get("/scd30", handleScd30Page);
  http_route_get("/api/scd30", handleApiScd30);
}
