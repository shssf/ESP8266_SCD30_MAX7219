#include "web_server.h"

static const char OTA_PAGE_MISSING_HTML[] PROGMEM =
    R"HTML(<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'/><title>HTTP OTA</title><link rel='stylesheet' href='/style.css'></head><body><h1>OTA page missing</h1><div><form method='GET' action='/'><button class='btn' type='submit'>Back</button></form></div><div class='card'><p>Upload filesystem image with <span class='mono'>ota_page.html</span> and <span class='mono'>style.css</span>.</p></div></body></html>)HTML";

void handleOtaPage()
{
  if (http_send_littlefs_file("/ota_page.html", "text/html; charset=utf-8"))
  {
    return;
  }

  http_send_P(500, "text/html; charset=utf-8", OTA_PAGE_MISSING_HTML);
}
