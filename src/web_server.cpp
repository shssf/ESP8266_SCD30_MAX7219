#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <Updater.h>

#include "web_css.h"
#include "web_server.h"
#include "web_ui_pages.h"
#include "wifi_support.h"

static ESP8266WebServer s_server(80);
static bool s_server_started = false;
static bool s_fs_ready = false;

static bool check_update_auth()
{
  if (!s_server.authenticate("admin", get_setup_ap_pass()))
  {
    s_server.requestAuthentication();
    return false;
  }

  return true;
}

static void handle_updatefw_post(void)
{
  if (!check_update_auth())
  {
    return;
  }

  if (Update.hasError())
  {
    http_send(500, "text/plain", "Firmware update failed");
    return;
  }

  http_send(200, "text/plain", "Firmware update OK. Press Restart controller to apply.");
}

static void handle_updatefw_upload(void)
{
  HTTPUpload& upload = s_server.upload();

  if (upload.status == UPLOAD_FILE_START)
  {
    const size_t max_sketch_space = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if (!Update.begin(max_sketch_space, U_FLASH))
    {
      Update.printError(Serial);
    }
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
    {
      Update.printError(Serial);
    }
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (!Update.end(true))
    {
      Update.printError(Serial);
    }
  }
}

static void handle_updatefs_post(void)
{
  if (!check_update_auth())
  {
    return;
  }

  if (Update.hasError())
  {
    http_send(500, "text/plain", "FS update failed");
    return;
  }

  http_send(200, "text/plain", "FS update OK. Press Restart controller to apply.");
}

static void handle_updatefs_upload(void)
{
  HTTPUpload& upload = s_server.upload();

  if (upload.status == UPLOAD_FILE_START)
  {
    if (!Update.begin((size_t)0xFFFFFFFF, U_FS))
    {
      Update.printError(Serial);
    }
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
    {
      Update.printError(Serial);
    }
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (!Update.end(true))
    {
      Update.printError(Serial);
    }
  }
}

static void handle_reboot_post(void)
{
  if (!check_update_auth())
  {
    return;
  }

  http_send(200, "text/plain", "Rebooting...");
  delay(300);
  ESP.restart();
}

void http_start(void)
{
  s_fs_ready = LittleFS.begin();

  ui_register_main_routes();
  ui_ota_routes();
  ui_register_scd30_routes();
  ui_register_css_routes();
}

void http_tick(void)
{
  if (wifi_is_connected())
  {
    if (!s_server_started)
    {
      s_server.begin();
      s_server_started = true;
    }
    s_server.handleClient();
  }
  else
  {
    if (s_server_started)
    {
      s_server.stop();
      s_server_started = false;
    }
  }
}

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

void http_send(int code, const char* content_type, const char* body)
{
  s_server.send(code, content_type, body);
}

void http_send_P(int code, const char* content_type, PGM_P content)
{
  s_server.send_P(code, content_type, content);
}

bool http_send_littlefs_file(const char* fs_path, const char* content_type)
{
  if (!s_fs_ready)
  {
    return false;
  }

  File file = LittleFS.open(fs_path, "r");
  if (!file)
  {
    return false;
  }

  s_server.streamFile(file, content_type);
  file.close();
  return true;
}

void ui_ota_routes(void)
{
  s_server.on("/update", HTTP_POST, handle_updatefw_post, handle_updatefw_upload);
  s_server.on("/updatefs", HTTP_POST, handle_updatefs_post, handle_updatefs_upload);
  s_server.on("/reboot", HTTP_POST, handle_reboot_post);
  http_route_get("/ota", handleOtaPage);
}
