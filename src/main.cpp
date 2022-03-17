#include "mbed.h"

#include "SixPack.h"
#include "Events.h"

using namespace SixPackLib;

#include "TCA9555.h"
#include "TLC59116.h"

static const uint16_t FirmwareVersion = 0x0010;
static const uint16_t DeviceType = 0x0010;

static const PinName SDA_PIN = PB_7;
static const PinName SCL_PIN = PB_6;
static const PinName STATUS_LED = PB_0;
static const PinName COMMS_LED = PB_1;
static const PinName CAN_TXD = PA_11;
static const PinName CAN_RXD = PA_12;
static const PinName SERIAL_RX_PIN = PA_3;
static const PinName SERIAL_TX_PIN = PA_2;
static const PinName BOOT_0_PIN = PH_3;

I2C i2c(SDA_PIN, SCL_PIN);
CAN can(CAN_TXD, CAN_RXD, 500000);

// TODO: ButtonManager needs own queue???
EventQueue queue(32 * EVENTS_EVENT_SIZE);

SixPack sixPack(&can);

TCA9555 tca9555(&i2c);
TLC59116 frontPanel(&i2c);
int toggle = 0;
int state = 0;

DigitalOut statusLed(STATUS_LED);
// InterruptIn button(BOOT_0_PIN);
// Timer timer;
// uint64_t lastPress;


AnalogIn internalTemperature(ADC_TEMP);
AnalogIn vrefint(ADC_VREF);

void measureTemperature() {
        int voltage = vrefint.read_u16() >> 4;
        int volt = __LL_ADC_CALC_VREFANALOG_VOLTAGE(voltage, LL_ADC_RESOLUTION_12B);

        int temp12 = internalTemperature.read_u16() >> 4;
        int temp = __LL_ADC_CALC_TEMPERATURE(volt, temp12, LL_ADC_RESOLUTION_12B);

        printf("Temperature: %d\r\nVoltage (mv): %d\r\n", temp, volt);
}

// void buttonPress() {
//     uint32_t now = std::chrono::duration_cast<std::chrono::milliseconds>(timer.elapsed_time()).count();
//     if(now > lastPress + 200 ){
//         state++;
//         state = state % 2;
//     }
//     lastPress = now;
// }

void setRelay(uint8_t index, bool state) {

    printf("SET RELAY: %d, %d\r\n", (int) index, (int) state);
    float brightness = state ? 1.0 : 0.0;
    frontPanel.setChannel(15 - index, brightness);
    tca9555.write(index, state);
    sixPack.send( Events::RelayUpdate(index, state) );
}

void resetRelays() {
    for(int i = 0; i<12; i++) {
        setRelay(i, false);
    }
}


int main() {
    sixPack.setStatusLed(COMMS_LED);
    printf("*** Relays module 12 channels ***\r\n");
    statusLed = 0;


    
    frontPanel.enable();
    resetRelays();
    tca9555.enableOutputs();

    sixPack.relayUpdateEvent([](SixPack::RelayUpdateStatus status) {
        printf("RELAY UPDATE STATUS !!! %d, %d\r\n", (int) status.index, (int) status.state);
        setRelay(status.index, status.state);
    });

    // for(int i = 0; i<12; i++) {
    //     setRelay(i, true);
    //     ThisThread::sleep_for(1s);
    //     setRelay(i, false);
    //     ThisThread::sleep_for(1s);
    // }


    queue.dispatch_forever();

    // LL_GetUID_Word0
}