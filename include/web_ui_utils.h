#pragma once

#include <Arduino.h>

String ui_uptime_str();
void ui_json_escape_cstr(const char* input, char* out, size_t out_size);
