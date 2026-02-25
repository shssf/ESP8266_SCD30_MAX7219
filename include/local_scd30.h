#pragma once

bool scd30_is_data_ready();
void scd30_get_values();
void scd30_read_values(float& co2Concentration, float& temperature, float& humidity);

void scd30_set_temp_offset_c(uint16_t value);
uint16_t scd30_get_temp_offset_c();

void scd30_start();
