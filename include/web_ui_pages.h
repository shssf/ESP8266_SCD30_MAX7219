#pragma once

void ui_register_main_routes();
void ui_ota_routes();
void ui_register_scd30_routes();

bool ui_scd30_api_alive();
void ui_scd30_api_heartbeat_stop();

void handleOtaPage();