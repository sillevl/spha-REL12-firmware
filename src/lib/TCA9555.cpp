#include "TCA9555.h"

TCA9555::TCA9555(I2C* i2c) {
    this->i2c = i2c;   
}

uint16_t TCA9555::read() {
    uint16_t result;
    uint8_t port0 = i2c_read_uint8(CMD_INPUT_PORT_0);
    uint8_t port1 = i2c_read_uint8(CMD_INPUT_PORT_1);
    result |= port0;
    result |= (port1 << 8);
    return result;
}


uint8_t TCA9555::i2c_read_uint8(char address) {
    char buffer[1];
    i2c->write(I2C_ADDRESS, &address, 1);
    i2c->read(I2C_ADDRESS, buffer, 1);
    return buffer[0];
}

void TCA9555::enableOutputs(){
    char buffer[3] = { CMD_CONFIGURATION_PORT_0, 0x00, 0x00 };
    i2c->write(I2C_ADDRESS, buffer, 3);
}

void TCA9555::write(uint8_t index, uint8_t state) {
    uint16_t status = read();
    if(state) {
        status |= 1UL << index;
    } else {
        status &= ~(1UL << index);
    }
    uint8_t outputPort0 = status & 0xFF;
    uint8_t outputPort1 = (status >> 8) & 0xFF;
    char buffer[3] = { CMD_OUTPUT_PORT_0, outputPort0, outputPort1 };
    i2c->write(I2C_ADDRESS, buffer, 3);
}