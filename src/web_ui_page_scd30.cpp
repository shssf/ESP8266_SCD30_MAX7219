#include <Arduino.h>

#include "web_server.h"
#include "web_ui_pages.h"

#include "local_scd30.h"

static bool api_heartbeat_started = false;
static const uint32_t SCD30_API_HEARTBEAT_TIMEOUT_MS = 10UL * 60UL * 1000UL;
static uint32_t api_last_request_ms = 0;

bool ui_scd30_api_alive()
{
  if (!api_heartbeat_started)
  {
    return true;
  }

  const uint32_t request_age_ms = millis() - api_last_request_ms;
  return request_age_ms <= SCD30_API_HEARTBEAT_TIMEOUT_MS;
}

void ui_scd30_api_heartbeat_stop()
{
  api_heartbeat_started = false;
  api_last_request_ms = 0;
}

static void handleApiScd30(void)
{
  api_heartbeat_started = true;
  api_last_request_ms = millis();
  http_send_header("Cache-Control", "no-store");

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
  http_send_header("Cache-Control", "public, max-age=120");
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
