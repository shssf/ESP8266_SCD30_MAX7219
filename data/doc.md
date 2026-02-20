# **Circuit Documentation**

## **Summary**

This circuit integrates a Wemos D1 Mini microcontroller with a MAX7219 8x32 LED Matrix and an SCD30 sensor. The Wemos D1 Mini serves as the central control unit, interfacing with the LED matrix to display information and communicating with the SCD30 sensor to gather environmental data such as CO2 concentration, temperature, and humidity. The circuit is powered through the Wemos D1 Mini, which supplies power to both the LED matrix and the sensor.

## **Component List**

1. **MAX7219 8x32 LED Matrix**  
   * **Description**: An LED matrix display module controlled by the MAX7219 driver, used for displaying text or graphics.  
   * **Pins**: Vcc, Gnd, Din, CS, CLK  
2. **SCD30**  
   * **Description**: A sensor module capable of measuring CO2 concentration, temperature, and humidity.  
   * **Pins**: Vin, GND, SCL, SDA, RDY, PWM, SEL  
3. **Wemos D1 Mini**  
   * **Description**: A compact microcontroller board based on the ESP8266, used for Wi-Fi connectivity and general-purpose processing.  
   * **Pins**: RST, A0, D0, D5, D6, D7, D8, 3V3, 5V, G, D4, D3, D2, D1, RX, TX

## **Wiring Details**

### **MAX7219 8x32 LED Matrix**

* **Vcc** is connected to the **5V** pin of the Wemos D1 Mini.  
* **Gnd** is connected to the **G** pin of the Wemos D1 Mini and the **GND** pin of the SCD30.  
* **Din** is connected to the **D7** pin of the Wemos D1 Mini.  
* **CS** is connected to the **D0** pin of the Wemos D1 Mini.  
* **CLK** is connected to the **D5** pin of the Wemos D1 Mini.

### ---

**SCD30**

* **Vin** is connected to the **3V3** pin of the Wemos D1 Mini.  
* **GND** is connected to the **G** pin of the Wemos D1 Mini and the **Gnd** pin of the MAX7219 8x32 LED Matrix.  
* **SCL** is connected to the **D1** pin of the Wemos D1 Mini.  
* **SDA** is connected to the **D2** pin of the Wemos D1 Mini.

### ---

**Wemos D1 Mini**

* **5V** is connected to the **Vcc** pin of the MAX7219 8x32 LED Matrix.  
* **3V3** is connected to the **Vin** pin of the SCD30.  
* **G** is connected to the **Gnd** pin of the MAX7219 8x32 LED Matrix and the **GND** pin of the SCD30.  
* **D7** is connected to the **Din** pin of the MAX7219 8x32 LED Matrix.  
* **D0** is connected to the **CS** pin of the MAX7219 8x32 LED Matrix.  
* **D5** is connected to the **CLK** pin of the MAX7219 8x32 LED Matrix.  
* **D1** is connected to the **SCL** pin of the SCD30.  
* **D2** is connected to the **SDA** pin of the SCD30.

## **Code Documentation**

There is no embedded code provided for this circuit. The Wemos D1 Mini can be programmed using the Arduino IDE or other compatible platforms to control the LED matrix and read data from the SCD30 sensor. The code would typically involve initializing the I2C communication for the SCD30 and SPI communication for the MAX7219, reading sensor data, and updating the display accordingly.