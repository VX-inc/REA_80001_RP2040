#include <Adafruit_NeoPixel.h>
#include <ACAN2040.h>

#define LED_STATUS_PIN 9
#define LED_STATUS_ADDRESS 0
#define PSU_CONNECT_OUTPUT_PIN 2
#define PSU_STANDBY_PIN 3
#define PSU_EN_12V_PIN 4
#define CONNECT_INPUT_PIN 5

#define CAN_STUFFING_FRAME 0xAA
#define CAN_IDENTIFIER 0x0A

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


enum CANDataType {
  CAN_PSU_VOLTAGE = 1,
  CAN_TEST_PATTERN = 2
};

PSUState psuState = PSU_POWER_OFF;

Adafruit_NeoPixel status_led = Adafruit_NeoPixel(1, LED_STATUS_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  delay(2000);
  initializeSerial();
  initializePSUPins();
  powerStateMachineCommand(PSU_POWER_OFF);
  initializeStatusLED();
  can2040.begin();
}


void loop() {
  checkCANMessages();
  serialParser();
}