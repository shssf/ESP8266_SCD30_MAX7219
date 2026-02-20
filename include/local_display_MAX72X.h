#pragma once

void MAX72X_start();
void MAX72X_print(float o2Concentration, float temperature, float humidity);
bool MAX72X_tick();