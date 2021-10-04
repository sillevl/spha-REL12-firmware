#pragma once

#include "mbed.h"

class TCA9555 {
    public:
    TCA9555(I2C* i2c);

    uint16_t read();
    void write(uint8_t index, uint8_t state);
    void enableOutputs();

    private:
    I2C* i2c;

    uint8_t i2c_read_uint8(char address);

    static const char I2C_ADDRESS = 0x20 << 1;
    static const char CMD_INPUT_PORT_0 = 0x00;
    static const char CMD_INPUT_PORT_1 = 0x01;
    static const char CMD_OUTPUT_PORT_0 = 0x02;
    static const char CMD_OUTPUT_PORT_1 = 0x03;
    static const char CMD_POLARITY_INVERSION_PORT_0 = 0x04;
    static const char CMD_POLARITY_INVERSION_PORT_1 = 0x05;
    static const char CMD_CONFIGURATION_PORT_0 = 0x06;
    static const char CMD_CONFIGURATION_PORT_1 = 0x07;
};
