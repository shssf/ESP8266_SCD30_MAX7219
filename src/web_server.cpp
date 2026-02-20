#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>


#include "web_server.h"
#include "web_css.h"
#include "web_ui_pages.h"

static ESP8266WebServer s_server(80);
static ESP8266HTTPUpdateServer s_updater;

void http_start(void)
{
  ui_register_main_routes();
  ui_ota_routes();
  ui_register_scd30_routes();
  ui_register_css_routes();

  s_server.begin();
}

void http_tick(void)
{
  s_server.handleClient();
}

/* -------- Routing -------- */
void http_route_get(const char* path, HttpHandlerFn handler)
{
  s_server.on(path, HTTP_GET, handler);
}

void http_route_any(const char* path, HttpHandlerFn handler)
{
  s_server.on(path, HTTP_ANY, handler);
}

void http_set_not_found(HttpHandlerFn handler)
{
  s_server.onNotFound(handler);
}

/* -------- Response helpers -------- */
void http_send(int code, const char* content_type, const char* body)
{
  s_server.send(code, content_type, body);
}

void http_send_P(int code, const char* content_type, PGM_P content)
{
    s_server.send_P(code, content_type, content);
}

void handleOtaPage(void);
void ui_ota_routes(void)
{
  http_route_get("/ota", handleOtaPage);
  s_updater.setup(&s_server, "/update", "admin", "12345678");
}
