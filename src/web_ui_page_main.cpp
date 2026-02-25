#include <ESP8266WiFi.h>
#include <LittleFS.h>

#include "web_server.h"
#include "web_ui_pages.h"
#include "web_ui_utils.h"
#include "wifi_support.h"

static void handleMainApi(void)
{
  const String host = String(get_hostname());
  const String ip = WiFi.localIP().toString();
  const String mac = WiFi.macAddress();
  const String ssid = WiFi.SSID();
  const int rssi = WiFi.RSSI();
  const int ch = WiFi.channel();

  const uint32_t chipId = ESP.getChipId();
  const uint32_t flashSize = ESP.getFlashChipRealSize();
  const uint32_t cpu = ESP.getCpuFreqMHz();
  const uint32_t freeHeap = ESP.getFreeHeap();
  const uint8_t heapFrag = ESP.getHeapFragmentation();
  const uint32_t maxFreeBlock = ESP.getMaxFreeBlockSize();

  const String resetReason = ESP.getResetReason();

  FSInfo fsInfo;
  const bool fsInfoOk = LittleFS.info(fsInfo);

  char chipIdHex[16];
  snprintf(chipIdHex, sizeof(chipIdHex), "0x%06X", chipId);

  char build[32];
  snprintf(build, sizeof(build), "%s %s", __DATE__, __TIME__);

  char hostEsc[96];
  char ipEsc[64];
  char macEsc[64];
  char ssidEsc[128];
  char uptimeEsc[64];
  char resetReasonEsc[96];

  ui_json_escape_cstr(host.c_str(), hostEsc, sizeof(hostEsc));
  ui_json_escape_cstr(ip.c_str(), ipEsc, sizeof(ipEsc));
  ui_json_escape_cstr(mac.c_str(), macEsc, sizeof(macEsc));
  ui_json_escape_cstr(ssid.c_str(), ssidEsc, sizeof(ssidEsc));
  ui_json_escape_cstr(ui_uptime_str().c_str(), uptimeEsc, sizeof(uptimeEsc));
  ui_json_escape_cstr(resetReason.c_str(), resetReasonEsc, sizeof(resetReasonEsc));

  char json[900];
  snprintf(json,
           sizeof(json),
           "{\"build\":\"%s\",\"hostname\":\"%s\",\"ip\":\"%s\",\"mac\":\"%s\","
           "\"ssid\":\"%s\",\"rssi_dbm\":%d,\"channel\":%d,\"chip_id\":\"%s\","
           "\"cpu_mhz\":%lu,\"flash_size_bytes\":%lu,\"uptime\":\"%s\","
           "\"free_heap_bytes\":%lu,\"heap_fragmentation_pct\":%u,\"max_free_block_bytes\":%lu,"
           "\"reset_reason\":\"%s\",\"fs_total_bytes\":%lu,\"fs_used_bytes\":%lu}",
           build,
           hostEsc,
           ipEsc,
           macEsc,
           ssidEsc,
           rssi,
           ch,
           chipIdHex,
           (unsigned long)cpu,
           (unsigned long)flashSize,
           uptimeEsc,
           (unsigned long)freeHeap,
           (unsigned)heapFrag,
           (unsigned long)maxFreeBlock,
           resetReasonEsc,
           fsInfoOk ? (unsigned long)fsInfo.totalBytes : 0UL,
           fsInfoOk ? (unsigned long)fsInfo.usedBytes : 0UL);

  http_send(200, "application/json; charset=utf-8", json);
}

static void handleMainPage(void)
{
  if (http_send_littlefs_file("/main_page.html", "text/html; charset=utf-8"))
  {
    return;
  }

  String h;
  h.reserve(512);
  h += ui_html_head("Wemos Control");
  h += "<h1>Main page missing</h1>";
  h += "<div class='card'><p>Upload filesystem image with <span class='mono'>main_page.html</span> and <span "
       "class='mono'>style.css</span>.</p></div>";
  h += "<div><form method='GET' action='/ota'><button class='btn' type='submit'>Open OTA</button></form></div>";
  h += ui_html_tail();
  http_send(500, "text/html; charset=utf-8", h.c_str());
}

void ui_register_main_routes(void)
{
  http_route_get("/", handleMainPage);
  http_route_get("/api/main", handleMainApi);
  http_route_any("/reset", wifi_forget_credentials);
}
