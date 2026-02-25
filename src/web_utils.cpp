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

void ui_json_escape_cstr(const char* input, char* out, size_t out_size)
{
  if (out_size == 0)
  {
    return;
  }

  size_t out_pos = 0;
  const char* src = input ? input : "";

  for (size_t i = 0; src[i] != '\0' && out_pos + 1 < out_size; ++i)
  {
    const char c = src[i];
    if (c == '"' || c == '\\')
    {
      if (out_pos + 2 >= out_size)
      {
        break;
      }
      out[out_pos++] = '\\';
      out[out_pos++] = c;
      continue;
    }

    if (c == '\n')
    {
      if (out_pos + 2 >= out_size)
      {
        break;
      }
      out[out_pos++] = '\\';
      out[out_pos++] = 'n';
      continue;
    }

    if (c == '\r')
    {
      if (out_pos + 2 >= out_size)
      {
        break;
      }
      out[out_pos++] = '\\';
      out[out_pos++] = 'r';
      continue;
    }

    out[out_pos++] = c;
  }

  out[out_pos] = '\0';
}
