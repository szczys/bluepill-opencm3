# USART RX demonstration

Setup up USART2 to output on pin A3 at 115200 8N1. When the chracter 'L' is received the LED will be toggled.

## Sending with a USB to serial dongle

```
sudo chmod 777 /dev/ttyUSB1
stty -F /dev/ttyUSB1 115200
echo "L" > /dev/ttyUSB1
```
