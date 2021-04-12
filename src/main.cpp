#include "mbed.h"
#include "lib/ButtonManager.h"

#include "SixPack.h"

using namespace SixPackLib;

#include "TLC59108.h"
#include "tph.h"

static const uint16_t FirmwareVersion = 0x0010;
static const uint16_t DeviceType = 0x0010;

static const PinName SDA_PIN = PB_7;
static const PinName SCL_PIN = PB_6;
static const PinName STATUS_LED = PB_0;
static const PinName CAN_TXD = PA_11;
static const PinName CAN_RXD = PA_12;
static const PinName SERIAL_RX_PIN = PA_3;
static const PinName SERIAL_TX_PIN = PA_2;
static const PinName VIN_ADC = PA_0;

static const PinName BUTTONS[6] = { PA_5, PA_6, PA_7, PA_8, PA_9, PA_10 };

I2C i2c(SDA_PIN, SCL_PIN);
CAN can(CAN_TXD, CAN_RXD, 500000);

AnalogIn busVoltage(VIN_ADC);

static const int I2C_ADDRESS = 0x80;
TLC59108 leds(&i2c,I2C_ADDRESS);
TPH tph(i2c);

// TODO: ButtonManager needs own queue???
EventQueue queue(32 * EVENTS_EVENT_SIZE);
ButtonManager buttons(&queue, BUTTONS);

SixPack sixPack(&can);

int main() {
    printf("*** Niko 4/6 way button ***\r\n");

    busVoltage.set_reference_voltage(3.3f);
    leds.enable();
    for(int i = 0; i < 6; i++) {
        leds.setChannel(i, 0);
    }
    

    sixPack.setStatusLed(STATUS_LED);
    sixPack.setDeviceType(DeviceType);
    sixPack.setFirmwareVersion(FirmwareVersion);

    sixPack.setTPHCallback([]() -> SixPack::TPH {
        TPHValues values =  tph.getValues();
        return { (float) values.temperature, (float) values.humidity, (float) values.pressure };
    });

    sixPack.setBusVoltageCallback([]() -> float {
        return busVoltage.read_voltage() * ((200.0f + 10.0f) / 10.0f) * 1.02f;  // calibration factor (tolerance)
    });
    
    buttons.onStateChange([] (uint8_t index, ButtonManager::ButtonState state, uint32_t time) {
        sixPack.buttonUpdate( { index, (SixPack::ButtonState) state, time } );
    });

    // TODO: feedback leds
    sixPack.feedbackLedsEvent([](SixPack::FeedbackLedStatus status){
        uint8_t gamma_lookup[] = { 0, 1, 2, 5, 10, 16, 25, 36, 50, 68, 89, 114, 142, 175, 213, 255 };
        float brightness = gamma_lookup[status.brightness >> 4] / 255.0;
        leds.setChannel(status.index, brightness);
    });

    queue.dispatch_forever();
}