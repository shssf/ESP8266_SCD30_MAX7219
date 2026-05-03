#pragma once

#include <Arduino.h>

class LocalDiagPrint : public Print
{
public:
	void begin(unsigned long baud);
	size_t write(uint8_t value) override;
	size_t write(const uint8_t* buffer, size_t size) override;
};

extern LocalDiagPrint Diag;
