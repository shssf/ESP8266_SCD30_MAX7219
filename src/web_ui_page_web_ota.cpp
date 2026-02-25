#include "web_server.h"
#include "web_ui_utils.h"

void handleOtaPage()
{
  if (http_send_littlefs_file("/ota_page.html", "text/html; charset=utf-8"))
  {
    return;
  }

  String h;
  h.reserve(512);
  h += ui_html_head("HTTP OTA");
  h += "<h1>OTA page missing</h1>";
  h += "<div class='card'><p>Upload filesystem image with <span class='mono'>ota_page.html</span> and <span "
       "class='mono'>style.css</span>.</p></div>";
  h += "<div><form method='GET' action='/'><button class='btn' type='submit'>Back</button></form></div>";
  h += ui_html_tail();
  http_send(500, "text/html; charset=utf-8", h.c_str());
}
