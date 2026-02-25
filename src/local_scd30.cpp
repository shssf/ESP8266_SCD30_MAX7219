#include <SensirionCore.h>
#include <SensirionI2cScd30.h>
#include <Wire.h> // I2C controller

#include "local_scd30.h"

SensirionI2cScd30 sensor;

static float co2 = -1.0f;
static float temperature = -1.0f;
static float humidity = -1.0f;
static uint16_t temp_offset_c = 530; // temperature correction for -5.3 C

#define MEASUREMENT_INTERVAL  30  // in seconds
#define TEMP_OFFSET_C         530 // centi-degrees, sensor subtracts this from measured temperature (31.7 -> 26.4)
#define AMBIENT_PRESSURE_MBAR 1010
#define AUTOMATIC_CALIBRATION 0

static void check_error(int16_t error)
{
  static char sensor_err_buf[64];

  if (error != NoError)
  {
    errorToString(error, sensor_err_buf, sizeof(sensor_err_buf));
  }
}

static void dump_scd30()
{
  int16_t error = NoError;
  uint16_t aux_val = 0;

  // HW Information dump
  Serial.printf("\n==== SCD30 sensor details (CO2, Temp, Humid) ====\n");

  uint8_t major = 0;
  uint8_t minor = 0;
  error = sensor.readFirmwareVersion(major, minor);
  check_error(error);
  Serial.printf("SCD30 Firmware version: %u.%u\n", major, minor);

  error = sensor.getMeasurementInterval(aux_val);
  check_error(error);
  Serial.printf("Measurement interval: %u s\n", aux_val);

  error = sensor.getDataReady(aux_val);
  check_error(error);
  Serial.printf("DataReady status: %u\n", aux_val);

  error = sensor.getAutoCalibrationStatus(aux_val);
  check_error(error);
  Serial.printf("Automatic calibration status (ASC): %u\n", aux_val);

  error = sensor.getForceRecalibrationStatus(aux_val);
  check_error(error);
  Serial.printf("The configured CO₂ reference concentration: %u ppm\n", aux_val);

  error = sensor.getTemperatureOffset(aux_val);
  check_error(error);
  Serial.printf("The configured temperature offset: %f C\n", float(aux_val) / 100.0f);

  error = sensor.getAltitudeCompensation(aux_val);
  check_error(error);
  Serial.printf("The configured altitude: %u m\n", aux_val);

  Serial.printf("==== End sensor details ====\n");
}

bool scd30_is_data_ready()
{
  uint16_t flag = 0;
  int16_t error = sensor.getDataReady(flag);
  check_error(error);

  if (flag)
  {
    return true;
  }

  return false;
}

void scd30_read_values(float& co2val, float& temp, float& humid)
{
  co2val = co2;
  temp = temperature;
  humid = humidity;
}

void scd30_get_values()
{
  int16_t error = sensor.readMeasurementData(co2, temperature, humidity);
  check_error(error);

  if (error != NoError)
  {
    co2 = -1.f;
    temperature = -1.f;
    humidity = -1.f;
  }
  else
  {
    // apply temperature offset
    // temperature -= float(TEMP_OFFSET_C) / 100.0f;
  }
}

void scd30_set_temp_offset_c(uint16_t value)
{
  int16_t error = sensor.setTemperatureOffset(value);
  check_error(error);
  temp_offset_c = value;
}

uint16_t scd30_get_temp_offset_c()
{
  int16_t error = sensor.getTemperatureOffset(temp_offset_c);
  check_error(error);

  return temp_offset_c;
}

void scd30_start()
{
  int16_t error = NoError;

  Wire.begin(); // I2C network
  sensor.begin(Wire, SCD30_I2C_ADDR_61);

  sensor.stopPeriodicMeasurement();
  sensor.softReset();
  delay(2000);

  dump_scd30();

  error = sensor.setMeasurementInterval(MEASUREMENT_INTERVAL);
  check_error(error);

  //   error = sensor.setAltitudeCompensation(100); // set surrent height in meters
  //   check_error(error);

  //   error = sensor.forceRecalibration(575); // set minimum which had seen in fresh air
  //   check_error(error);

  error = sensor.activateAutoCalibration(AUTOMATIC_CALIBRATION);
  check_error(error);

  error = sensor.setTemperatureOffset(temp_offset_c);
  check_error(error);

  error = sensor.startPeriodicMeasurement(AMBIENT_PRESSURE_MBAR);
  check_error(error);
}
