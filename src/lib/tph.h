#pragma once

#include "mbed.h"
#include "BME280.h"

struct TPHValues {
    double temperature;
    double humidity;
    double pressure;
};

class TPH{
    public:
    
    TPH(I2C& i2c);
    TPH(I2C& i2c, int address);
    
    TPHValues getValues();
    
    protected:
    BME280 bme280;

};