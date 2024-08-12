#include <Adafruit_NeoPixel.h>
#include <ACAN2040.h>
#include <Wire.h>
#include "Adafruit_HUSB238.h"

#define LED_STATUS_PIN 9
#define LED_STATUS_ADDRESS 0
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define PSU_CONNECT_OUTPUT_PIN 2
#define PSU_STANDBY_PIN 3
#define PSU_EN_12V_PIN 4
#define CONNECT_INPUT_PIN 5
#define DEVICE_TYPE_PIN 18

#define CAN_STUFFING_FRAME 0xAA
#define CAN_IDENTIFIER 0x0A

#define WIRE Wire

const uint8_t PIONUM0 = 0;
const uint8_t TXPIN0 = 7;
const uint8_t RXPIN0 = 6;
const uint32_t BITRATE0 = 125000UL;
const uint32_t SYSCLK = F_CPU;

void my_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg);
char *msg_to_str(struct can2040_msg *msg);

ACAN2040 can2040(PIONUM0, TXPIN0, RXPIN0, BITRATE0, SYSCLK, my_cb);
bool got_msg = false;
struct can2040_msg tx_msg, rx_msg;
struct can2040_stats can_stats;

enum PSUState {
  PSU_POWER_OFF = 1,
  PSU_20V = 2,
  PSU_12V = 3,
  PSU_5V = 4
};

enum PSUStatus {
  PSU_OK = 0,
  PSU_OVER_CURRENT = 1
};

enum CANDataType {
  CAN_PSU_VOLTAGE = 1,
  CAN_TEST_PATTERN = 2,
  CAN_CURRENT_REQUEST = 3,
  CAN_CURRENT_ZERO_REQUEST = 4,
  CAN_CURRENT_DATA = 5,
  CAN_PSU_STATUS = 6
};

enum DeviceType {
  DEVICE_UNKNOWN = 0,
  DEVICE_ATTACHED_PSU = 1,
  DEVICE_STANDALONE_PSU = 2
};

PSUState psuState = PSU_POWER_OFF;

Adafruit_NeoPixel status_led = Adafruit_NeoPixel(1, LED_STATUS_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  delay(100);
  initializeSerial();
  checkDeviceType();
  can2040.begin();
  initI2C();
  initalize_USB_PD();
  initCurrentSense();
  initializeStatusLED();
  initializePSUPins();
  powerStateMachineCommand(PSU_POWER_OFF);
}


void loop() {
  slottedLoop();
}


//Functions that run once every 100ms
void Slot_100ms() {
  refreshStatusLED();
}

//Functions that run once every 10ms
void Slot_10ms() {
  checkCANMessages();
  serialParser();
}

//Functions that run once every loop (the fastest possible)
void Slot_EveryLoop() {
}
