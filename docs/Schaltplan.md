# Schaltplan

![Schaltplan](/pics/Schematic.jpg)

## Anschlussbelegung

### GY-521

- D4 --> SCL des GY-521
- D3 --> SDA des GY-521
- 5V --> VCC
- GND --> GND
- restliche ports unbelegt
- Vorwiderstand der GY LED entfernen! (Ruhestrom) 

### Widerstände

- BAT43 Schottky Diode (Kathode zu D0) oder 330 Ohm zwischen D0 und RST
- 230k Ohm von A0 --> 5V
- 4k7 Ohm von D6 --> 5V

### DS18B20

- DQ geht an D6
- VCC and 5V
- GND --> GND

### Wemos

- Eingangs - Diode entfernen (Kurzschluss über Lipomodul!)
- Schalter / Reed zwischen +5V und Lipomodul `+OUT`
- GND --> GND

### Lipo Lademodul

- Akku an `B+` und `B-`
- GND --> GND
- `+OUT` Schalter zu +5V Wemos
- `+IN` an USB +5V des Wemos (optional)