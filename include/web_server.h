#pragma once

#include <pgmspace.h>

void http_start(void);
void http_tick(void);

typedef void (*HttpHandlerFn)(void); // std::function<void(void)>

void http_route_get(const char* path, HttpHandlerFn handler);
void http_route_any(const char* path, HttpHandlerFn handler);
void http_set_not_found(HttpHandlerFn handler);

void http_send(int code, const char* content_type, const char* body);
void http_send_P(int code, const char* content_type, PGM_P content);
bool http_send_littlefs_file(const char* fs_path, const char* content_type);
