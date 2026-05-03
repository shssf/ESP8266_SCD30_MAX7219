#include "local_diag.h"

// please do not use Diag inside this class methods to avoid recursion
LocalDiagPrint Diag;

void LocalDiagPrint::begin(unsigned long baud)
{
  Serial.begin(baud);
}

size_t LocalDiagPrint::write(uint8_t value)
{
  return write(&value, 1); // keep all output paths in the buffer write overload
}

size_t LocalDiagPrint::write(const uint8_t* buffer, size_t size)
{
  return Serial.write(buffer, size);
}
