#!/bin/bash -v

# Display "**" on Zen Cape's 14-seg 2-digit display.

# List I2C devices
i2cdetect -l


# Enable Linux I2C support for Linux I2C bus #1 (HW bus 1)
config-pin P9_18 i2c
config-pin P9_17 i2c

# List I2C devices
i2cdetect -l

# Display GPIO chip's data again
i2cdump -y 1 0x20

# Set direction of both ports on I2C GPIO extender 
i2cset -y 1 0x20 0x00 0x00
i2cset -y 1 0x20 0x01 0x00

# Set bits to turn on for '*'
#   Lower 8-bits (0xFF for all segments on, some unused)
i2cset -y 1 0x20 0x14 0x1E
#   Upper 8-bits (0xFF for all segments on, some unused)
i2cset -y 1 0x20 0x15 0x78

# Display GPIO chip's data again
i2cdump -y 1 0x20

# Turn on both digits
echo 61 > /sys/class/gpio/export
echo 44 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio61/direction
echo out > /sys/class/gpio/gpio44/direction
echo 1 > /sys/class/gpio/gpio61/value
echo 1 > /sys/class/gpio/gpio44/value



i2cset -y 1 0x20 0x14 0x2A
i2cset -y 1 0x20 0x15 0x54

