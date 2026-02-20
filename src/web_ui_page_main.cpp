#include <ESP8266WiFi.h>

#include "web_server.h"
#include "web_ui_pages.h"
#include "web_ui_utils.h"
#include "wifi_support.h"

struct WebButton
{
  const char* text;        /* button caption                             */
  const char* href;        /* target URL                                 */
  const char* method;      /* "GET" or "POST"                            */
  const char* css;         /* visual class: "primary", "danger", ...     */
  const char* description; /* human-readable description                 */
};

static String ui_build_button_cell(const WebButton* b)
{
  String cell;
  cell.reserve(256);
  cell += "<td>";
  if (strcmp(b->method, "POST") == 0)
  {
    cell += "<form method='POST' action='";
    cell += b->href;
    cell += "'>";
    cell += "<button class='btn ";
    cell += b->css;
    cell += "' type='submit'>";
    cell += b->text;
    cell += "</button></form>";
  }
  else
  {
    cell += "<button class='btn ";
    cell += b->css;
    cell += "' ";
    cell += "onclick=\"location.href='";
    cell += b->href;
    cell += "'\">";
    cell += b->text;
    cell += "</button>";
  }
  cell += "</td>";
  return cell;
}

String ui_build_buttons_table()
{
  const WebButton buttons[] = {
      {"SCD30",       "/scd30",      "GET",  "warning", "Temp, Hum, CO2"},
      {"Web OTA",     "/ota",        "GET",  "warning", "Styled OTA page; POST goes to /update (user: admin, pass: SETUP_AP_PASS)."},
      {"RESET WiFi",  "/reset",      "POST", "danger",  "Erase saved WiFi credentials and reboot the device."                      }
  };
  const size_t count = sizeof(buttons) / sizeof(buttons[0]);

  String h;
  h.reserve(2048);
  h += "<table class='btn-table'>";
  h += "<tr><th>Action</th><th>Path</th><th>Description</th></tr>";

  for (size_t i = 0; i < count; ++i)
  {
    const WebButton* b = &buttons[i];
    h += "<tr>";
    h += ui_build_button_cell(b);
    h += "<td class='mono'><a class='link' href='";
    h += b->href;
    h += "'>";
    h += b->href;
    h += "</a></td>";
    h += "<td>";
    h += b->description;
    h += "</td>";
    h += "</tr>";
  }

  h += "</table>";
  return h;
}

static void handleMain(void)
{
  String h;
  h.reserve(20000);

  const String host = String(get_hostname());
  const String ip = WiFi.localIP().toString();
  const String mac = WiFi.macAddress();
  const int rssi = WiFi.RSSI();
  const int ch = WiFi.channel();

  const uint32_t chipId = ESP.getChipId();
  const uint32_t flashSize = ESP.getFlashChipRealSize();
  const uint32_t cpu = ESP.getCpuFreqMHz();

  h += ui_html_head("Wemos Control");
  h += "<h1>Wemos D1 mini + SCD30 (built " + String(__DATE__) + " " + String(__TIME__) + ")</h1>";

  h += "<div class='card'><table>";
  h += "<tr><td>Hostname</td><td>" + host + "</td></tr>";
  h += "<tr><td>IP / GW / Mask</td><td>" + ip + "</td></tr>";
  h += "<tr><td>MAC</td><td>" + mac + "</td></tr>";
  h += "<tr><td>WiFi</td><td>SSID: " + String(WiFi.SSID()) + ", RSSI: " + String(rssi) + " dBm, Channel: " + String(ch) + "</td></tr>";

  h += "<tr><td>Chip ID</td><td>0x" + String(chipId, HEX) + "</td></tr>";
  h += "<tr><td>CPU freq</td><td>" + String(cpu) + " MHz</td></tr>";
  h += "<tr><td>Flash size</td><td>" + String(flashSize) + " B</td></tr>";

  h += "<tr><td>Uptime</td><td>" + ui_uptime_str() + "</td></tr>";
  h += "</table></div>";

  h += ui_build_buttons_table();
  h += "</div>";

  h += ui_html_tail();
  http_send(200, "text/html; charset=utf-8", h.c_str());
}

void ui_register_main_routes(void)
{
  http_route_get("/", handleMain);
  http_route_any("/reset", wifi_forget_credentials);
}
