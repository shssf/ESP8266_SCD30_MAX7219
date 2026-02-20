#pragma once

void wifi_start();
void wifi_tick();
bool wifi_is_connected();
void wifi_forget_credentials();

const char* get_hostname();
