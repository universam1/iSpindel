# 电路图

![Schematic](/pics/schematic-en.png)

## 连接分配

### GY-521

- D4 -> SCL of the GY-521
- D3 -> SDA of the GY-521
- 5V -> VCC
- GND -> GND
- 剩余的端口不被占用
- 拆下 GY LED 的电阻！(不再需要)

### 电阻

- BAT43 或者更好的肖特基二极管 (负极 to D0) 或在 D0 与 RST 之间的 330 欧姆电阻
- 230k ohms of A0 -> 5V
- 4k7 ohms of D6 -> 5V

### DS18B20

- DQ goes to D6
VCC and 5V
- GND -> GND

### Wemos

- 移除输入二极管 (通过 Lipomodul 充电板短路!)
- 切换 / 簧片 在 + 5V 和 Lipomodul `+ OUT` 之间
- GND -> GND

### 锂电池充电模块

- 电池 to `B +` and `B-`
- GND -> GND
- `+ OUT` switch to + 5V Wemos D1 mini
- `+ IN` to USB + 5V of the Wemos D1 mini (选项)
