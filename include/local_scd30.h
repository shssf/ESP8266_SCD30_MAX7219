#pragma once

bool scd30_is_data_ready();
void scd30_get_values();
void scd30_read_values(float& co2Concentration, float& temperature, float& humidity);
void scd30_start();
