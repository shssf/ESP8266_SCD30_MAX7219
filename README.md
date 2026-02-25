# ESP8266_SCD30_MAX7219
ESP8266 + SCD30 + MAX7219

## SCD30 temperature offset without reflashing

- Open `/scd30` page in browser.
- In **Temperature offset** block set value in centi-degrees (`100 = 1.00°C`).
- Press **Apply** to send new value immediately to SCD30.

API:

- Read current offset: `/api/scd30/temp_offset`
- Set new offset: `/api/scd30/temp_offset?value=530`

Formula:

- `offset_centi = (temperature_scd30 - reference_temperature) * 100`
