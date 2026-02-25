#include "web_server.h"

void handleOtaPage()
{
  http_send_header("Cache-Control", "public, max-age=120");
  if (http_send_littlefs_file("/ota_page.html", "text/html; charset=utf-8"))
  {
    return;
  }

  http_send(404, "text/plain; charset=utf-8", "ota_page.html not found in LittleFS");
}
