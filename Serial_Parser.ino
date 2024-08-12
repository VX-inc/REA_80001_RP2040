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
    if (strcmp(inputString, "scan") == 0) {
      runI2CScanner();
      validCommand = true;
    }
    if (strcmp(inputString, "psu") == 0) {
      readAndDisplayPSU();
      validCommand = true;
    }
    if (strcmp(inputString, "volt") == 0) {
      readInputVoltage();
      validCommand = true;
    }
    if (strcmp(inputString, "pd") == 0) {
      USB_PD_Print();
      validCommand = true;
    }
    if (strcmp(inputString, "cs") == 0) {
      printCurrentSenseVoltage();
      validCommand = true;
    }
    if (strcmp(inputString, "csc") == 0) {
      printCurrentContinuously();
      validCommand = true;
    }
    if (strcmp(inputString, "dt") == 0) {
      printDeviceType();
      validCommand = true;
    }

    if (validCommand == false) {
      Serial.println("Invalid Command");
      printCommands();
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
  Serial.println("scan : Run I2C Scanner");
  Serial.println("psu : Read and print all PSU Registers");
  Serial.println("volt : Read input voltage to PSU");
  Serial.println("pd : print the USB-C PD Profile");
  Serial.println("cs : print current sense information once");
  Serial.println("csc : print current sense current continuously for 30 seconds");
  Serial.println("dt : print device type");
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