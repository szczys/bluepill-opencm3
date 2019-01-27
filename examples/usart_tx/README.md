# USART TX demonstration

Setup up USART2 to output on pin A2 at 115200 8N1

## Receiving using a Serial to USB dongle

```
sudo chmod 777 /dev/ttyUSB1
stty -F /dev/ttyUSB1 115200
cat /dev/ttyUSB1
```
