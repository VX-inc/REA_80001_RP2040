void initializePSUPins() {
  pinMode(PSU_CONNECT_OUTPUT_PIN, OUTPUT);
  pinMode(PSU_STANDBY_PIN, OUTPUT);
  pinMode(PSU_EN_12V_PIN, OUTPUT);
  pinMode(CONNECT_INPUT_PIN, OUTPUT);
  digitalWrite(PSU_CONNECT_OUTPUT_PIN, LOW);
  digitalWrite(CONNECT_INPUT_PIN, LOW);
  digitalWrite(PSU_STANDBY_PIN, HIGH);
  digitalWrite(PSU_EN_12V_PIN, LOW);
}

void powerStateMachineCommand(PSUState commandedState) {
  psuState = commandedState;

  digitalWrite(PSU_CONNECT_OUTPUT_PIN, LOW);
  digitalWrite(CONNECT_INPUT_PIN, LOW);
  delay(100);

  if (psuState == PSU_POWER_OFF) {
    digitalWrite(PSU_STANDBY_PIN, HIGH);
    digitalWrite(PSU_EN_12V_PIN, LOW);
    //updateStatusLED(psuState);
    Serial.println("LED Power Off");
  }
  /*
  if (psuState == PSU_20V) {
    digitalWrite(PSU_STANDBY_PIN, HIGH);
    digitalWrite(PSU_EN_12V_PIN, LOW);
    digitalWrite(CONNECT_INPUT_PIN, HIGH);
    updateStatusLED(psuState);
    Serial.println("LED Power 20V Enabled");
  }
  */
  if (psuState == PSU_12V) {
    digitalWrite(PSU_STANDBY_PIN, LOW);
    digitalWrite(PSU_EN_12V_PIN, HIGH);
    delay(1);
    digitalWrite(PSU_CONNECT_OUTPUT_PIN, HIGH);
    //updateStatusLED(psuState);
    Serial.println("LED Power 12V Enabled");
  }
  if (psuState == PSU_5V) {
    digitalWrite(PSU_STANDBY_PIN, LOW);
    digitalWrite(PSU_EN_12V_PIN, LOW);
    ;
    delay(1);
    digitalWrite(PSU_CONNECT_OUTPUT_PIN, HIGH);
    //updateStatusLED(psuState);
    Serial.println("LED Power 5V Enabled");
  }
}

void initializeStatusLED() {
  status_led.begin();
  status_led.clear();
  status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(10, 10, 10));
  while(!status_led.canShow()){}
  status_led.show();
}

void updateStatusLED(PSUState commandedSupplyState) {
  if (commandedSupplyState == PSU_POWER_OFF) {
    status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(10, 10, 10));
  }
  if (commandedSupplyState == PSU_20V) {
    status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(0, 0, 10));
  }
  if (commandedSupplyState == PSU_12V) {
    status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(0, 10, 10));
  }
  if (commandedSupplyState == PSU_5V) {
    status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(0, 10, 0));
  }
  while(!status_led.canShow()){}
  status_led.show();
}

void my_cb(struct can2040* cd, uint32_t notify, struct can2040_msg* msg) {

  (void)(cd);

  switch (notify) {
    case CAN2040_NOTIFY_RX:
      //Serial.printf("cb: message received\n");
      rx_msg = *msg;
      got_msg = true;
      break;
    case CAN2040_NOTIFY_TX:
      Serial.printf("cb: message sent ok\n");
      break;
    case CAN2040_NOTIFY_ERROR:
      Serial.printf("cb: an error occurred\n");
      break;
    default:
      Serial.printf("cb: unknown event type\n");
      break;
  }
}

const int maxLength = 50;     // Maximum length of the input string
char inputString[maxLength];  // Array to hold the incoming string
int inputIndex = 0;           // Index to keep track of the current position in the array
bool stringComplete = false;

void serialParser() {
  if (stringComplete) {
    bool validCommand = false;

    if (strcmp(inputString, "0") == 0) {
      sendVoltageCommand(PSU_POWER_OFF);
      Serial.println("Turning off LED Power ");
      validCommand = true;
    }
    if (strcmp(inputString, "20V") == 0) {
      //setVoltage(PSU_20V);
      sendVoltageCommand(PSU_POWER_OFF);
      Serial.println("20V Mode Currently Disabled");
      Serial.println("Turning off LED Power");
      validCommand = true;
    }
    if (strcmp(inputString, "12V") == 0) {
      sendVoltageCommand(PSU_12V);
      Serial.println("12V LED Power Enabling");
      validCommand = true;
    }
    if (strcmp(inputString, "5V") == 0) {
      sendVoltageCommand(PSU_5V);
      Serial.println("5V LED Power Enabling");
      validCommand = true;
    }
    if (strcmp(inputString, "t") == 0) {
      sendTestPatternCommand();
      Serial.println("Starting/Stopping Test Pattern");
      validCommand = true;
    }

    if (validCommand == false) {
      Serial.println("Invalid Command");
    }

    memset(inputString, 0, sizeof(inputString));
    inputIndex = 0;
    stringComplete = false;
  }

  while (Serial.available()) {
    char inChar = (char)Serial.read();  // Read the incoming byte

    if (inChar == '\n') {
      inputString[inputIndex] = '\0';  // Null-terminate the string
      stringComplete = true;
    } else {
      if (inputIndex < maxLength - 1) {
        inputString[inputIndex++] = inChar;  // Add the incoming byte to the string
      }
    }
  }
}

void initializeSerial() {
  Serial.begin(115200);
  Serial.println("RP2040 Hardware Test Suite v0.1 Starting...");
  printCommands();
}

void printCommands() {
  Serial.println("The test commands are as follows:");
  Serial.println("0 : Turn off LED Power Supply");
  Serial.println("20V : Turn on 20V LED Power (currently not functional)");
  Serial.println("12V : Turn on 12V LED Power");
  Serial.println("5V : Turn on 5V LED Power");
  Serial.println("t : Run/Stop Test Pattern on LED Strip (power must be enabled first)");
}

void checkCANMessages() {
  if (got_msg) {
    got_msg = false;
    uint8_t ident = rx_msg.id;
    uint8_t CANMessageType = rx_msg.data[0];
    uint8_t parameter = rx_msg.data[1];
    // Serial.println(ident);
    // Serial.println(CANMessageType);
    // Serial.println(parameter);
    if (ident == CAN_IDENTIFIER) {
      if (CANMessageType == CAN_PSU_VOLTAGE) {
        if (parameter > 0 && parameter <= PSU_5V) {
          updatePowerState((PSUState)parameter);
        }
      }
    }
  }
}

void updatePowerState(PSUState commandedSupplyState) {
  Serial.print("Power Supply State: ");
  Serial.println(commandedSupplyState);
  powerStateMachineCommand(commandedSupplyState);
  psuState = commandedSupplyState;
}

void sendVoltageCommand(PSUState voltageState) {
  Serial.println("Sending Supply Voltage CAN Message.");
  updatePowerState(voltageState);
  tx_msg.id = CAN_IDENTIFIER;
  tx_msg.dlc = 8;
  tx_msg.data[0] = CAN_PSU_VOLTAGE;
  tx_msg.data[1] = voltageState;
  tx_msg.data[2] = CAN_STUFFING_FRAME;
  tx_msg.data[3] = CAN_STUFFING_FRAME;
  tx_msg.data[4] = CAN_STUFFING_FRAME;
  tx_msg.data[5] = CAN_STUFFING_FRAME;
  tx_msg.data[6] = CAN_STUFFING_FRAME;
  tx_msg.data[7] = CAN_STUFFING_FRAME;
  can2040.send_message(&tx_msg);
}

void sendTestPatternCommand() {
  Serial.println("Sending Start/Stop Test Pattern CAN Message.");
  tx_msg.id = CAN_IDENTIFIER;
  tx_msg.dlc = 8;
  tx_msg.data[0] = CAN_TEST_PATTERN;
  tx_msg.data[1] = CAN_STUFFING_FRAME;
  tx_msg.data[2] = CAN_STUFFING_FRAME;
  tx_msg.data[3] = CAN_STUFFING_FRAME;
  tx_msg.data[4] = CAN_STUFFING_FRAME;
  tx_msg.data[5] = CAN_STUFFING_FRAME;
  tx_msg.data[6] = CAN_STUFFING_FRAME;
  tx_msg.data[7] = CAN_STUFFING_FRAME;
  can2040.send_message(&tx_msg);
}