#ifndef WEB_UI_UTILS_H
#define WEB_UI_UTILS_H

#include <Arduino.h>

/** Convert millis() to "Xd HHh MMm SSs" string. */
String ui_uptime_str();

String ui_html_head(const char* title);

/** Build closing tags for BODY/HTML. */
String ui_html_tail(void);

/** Escape C-string for safe JSON string value embedding. */
void ui_json_escape_cstr(const char* input, char* out, size_t out_size);

#endif /* WEB_UI_UTILS_H */
