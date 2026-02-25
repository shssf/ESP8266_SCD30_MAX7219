#include "web_css.h"
#include "web_server.h"


static void handle_css_main()
{
  if (http_send_littlefs_file("/style.css", "text/css; charset=utf-8"))
  {
    return;
  }

  http_send(404, "text/plain; charset=utf-8", "style.css not found in LittleFS");
}

void ui_register_css_routes()
{
  http_route_get("/style.css", handle_css_main);
}