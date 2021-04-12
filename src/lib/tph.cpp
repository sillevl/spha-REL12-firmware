
#include "tph.h"

TPH::TPH(I2C& i2c) : bme280(i2c){
    bme280.getTemperature();
    bme280.getHumidity();
    bme280.getPressure();
}


TPH::TPH(I2C& i2c, int address): bme280(i2c, address) {
    bme280.getTemperature();
    bme280.getHumidity();
    bme280.getPressure();
}

TPHValues TPH::getValues() {
    TPHValues values;
    values.temperature = bme280.getTemperature();
    values.humidity = bme280.getHumidity();
    values.pressure = bme280.getPressure();
    return values;
}
