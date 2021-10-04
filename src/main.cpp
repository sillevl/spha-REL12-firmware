#include "mbed.h"

#include "SixPack.h"

using namespace SixPackLib;

#include "TCA9555.h"
#include "TLC59116.h"

static const uint16_t FirmwareVersion = 0x0010;
static const uint16_t DeviceType = 0x0010;

static const PinName SDA_PIN = PB_7;
static const PinName SCL_PIN = PB_6;
static const PinName STATUS_LED = PB_0;
static const PinName CAN_TXD = PA_11;
static const PinName CAN_RXD = PA_12;
static const PinName SERIAL_RX_PIN = PA_3;
static const PinName SERIAL_TX_PIN = PA_2;

I2C i2c(SDA_PIN, SCL_PIN);
CAN can(CAN_TXD, CAN_RXD, 500000);

// TODO: ButtonManager needs own queue???
EventQueue queue(32 * EVENTS_EVENT_SIZE);

SixPack sixPack(&can);

TCA9555 tca9555(&i2c);
TLC59116 frontPanel(&i2c);
int toggle = 0;


AnalogIn internalTemperature(ADC_TEMP);
AnalogIn vrefint(ADC_VREF);

void measureTemperature() {
        int voltage = vrefint.read_u16() >> 4;
        int volt = __LL_ADC_CALC_VREFANALOG_VOLTAGE(voltage, LL_ADC_RESOLUTION_12B);

        int temp12 = internalTemperature.read_u16() >> 4;
        int temp = __LL_ADC_CALC_TEMPERATURE(volt, temp12, LL_ADC_RESOLUTION_12B);

        printf("Temperature: %d\r\nVoltage (mv): %d\r\n", temp, volt);
}


int main() {
    printf("*** Relays module 6 channels ***\r\n");

    sixPack.setStatusLed(STATUS_LED, false);
    sixPack.setDeviceType(DeviceType);
    sixPack.setFirmwareVersion(FirmwareVersion);

    tca9555.enableOutputs();
    frontPanel.enable();

    for(int i = 0; i<6; i++) {
        frontPanel.setChannel(i, 1.0);
        ThisThread::sleep_for(200ms);
    }

    for(int i = 0; i<6; i++) {
        frontPanel.setChannel(i, 0.0);
        ThisThread::sleep_for(200ms);
    }

    tca9555.write(0, toggle);

    queue.call_every(60min, []() {
        tca9555.write(0, toggle);
        // led = toggle;
        toggle = !toggle;
    });

    queue.call_every(30s, [](){
        int voltage = vrefint.read_u16() >> 4;
        int volt = __LL_ADC_CALC_VREFANALOG_VOLTAGE(voltage, LL_ADC_RESOLUTION_12B);

        int temp12 = internalTemperature.read_u16() >> 4;
        int temp = __LL_ADC_CALC_TEMPERATURE(volt, temp12, LL_ADC_RESOLUTION_12B);

        printf("Temperature: %d\r\nVoltage (mv): %d\r\n", temp, volt);
    });

    queue.dispatch_forever();

    // LL_GetUID_Word0
}