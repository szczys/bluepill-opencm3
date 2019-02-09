# ADC demonstration

Reads analog value from PA0, streams data out USART2

Uses boilerplate code from [ADC doc of libopencm3](http://libopencm3.org/docs/latest/stm32f1/html/group__adc__file.html)

## Board Setup:

- Connect PA2,PA3 to RX,TX pins of serial-to-usb cable
- Connect outter legs of trimpot to VCC and GND, inner leg to PA0

## Receiving using a Serial to USB dongle

```
sudo chmod 777 /dev/ttyUSB1
stty -F /dev/ttyUSB1 115200
cat /dev/ttyUSB1
```
