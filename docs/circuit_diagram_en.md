# Circuit diagram

![Schematic](/pics/schematic-en.png)

## Connection assignment

### GY-521

- D4 -> SCL of the GY-521
- D3 -> SDA of the GY-521
- 5V -> VCC
- GND -> GND
- remaining ports unoccupied
- Remove the resistor of the GY LED! (Retirement)

### Resistors

- BAT43 or better schottky diode (cathode to D0) or 330 ohms resistor between D0 and RST
- 230k ohms of A0 -> 5V
- 4k7 ohms of D6 -> 5V

### DS18B20

- DQ goes to D6
VCC and 5V
- GND -> GND

### Wemos

- Remove the input diode (short circuit via Lipomodul!)
- Switch / Reed between + 5V and Lipomodul `+ OUT`
- GND -> GND

### Lipo Lademodul

- Battery to `B +` and `B-`
- GND -> GND
- `+ OUT` switch to + 5V Wemos D1 mini
- `+ IN` to USB + 5V of the Wemos D1 mini (optional)
