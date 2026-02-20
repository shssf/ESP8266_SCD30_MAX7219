#include "web_ui_utils.h"

String ui_uptime_str()
{
  unsigned long ms = millis();
  unsigned long s = ms / 1000UL;
  unsigned long m = s / 60UL;
  unsigned long h = m / 60UL;
  unsigned long d = h / 24UL;
  char buf[64];
  snprintf(buf, sizeof(buf), "%lud %02luh %02lum %02lus", d, h % 24UL, m % 60UL, s % 60UL);
  return String(buf);
}

String ui_html_head(const char* title)
{
    String h;
    h.reserve(512); // small buffer is enough now
    h += "<!DOCTYPE html><html><head><meta charset='utf-8'>";
    h += "<meta name='viewport' content='width=device-width,initial-scale=1'/>";
    h += "<title>";
    h += title ? title : "Wemos";
    h += "</title>";
    h += "<link rel='stylesheet' href='/assets/main.css'>";
    h += "</head><body>";
    return h;
}

String ui_html_tail(void)
{
  return String("</body></html>");
}
