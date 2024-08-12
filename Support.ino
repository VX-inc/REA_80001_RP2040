
void slottedLoop() {
  static uint32_t slot_100ms = 0;
  static uint32_t slot_10ms = 0;

  if (millis() >= slot_100ms) {
    Slot_100ms();
    while (millis() >= slot_100ms) {
      slot_100ms += 100;
    }
  }

  if (millis() >= slot_10ms) {
    Slot_10ms();
    while (millis() >= slot_10ms) {
      slot_10ms += 10;
    }
  }

  Slot_EveryLoop();
}

DeviceType deviceDetected = DEVICE_UNKNOWN;

void checkDeviceType() {
  pinMode(DEVICE_TYPE_PIN, INPUT_PULLUP);
  delay(1);
  PinStatus deviceTypeRead = digitalRead(DEVICE_TYPE_PIN);
  if (deviceTypeRead == LOW) {
    deviceDetected = DEVICE_STANDALONE_PSU;
    Serial.println("Device Detected: Standalone PSU");
  }
  if (deviceTypeRead == HIGH) {
    deviceDetected = DEVICE_ATTACHED_PSU;
    Serial.println("Device Detected: Attached PSU");
  }
}

DeviceType getDeviceType() {
  return deviceDetected;
}

void printDeviceType() {
  if (deviceDetected == DEVICE_STANDALONE_PSU) {
    Serial.println("Device Detected: Standalone PSU");
  }
  if (deviceDetected == DEVICE_ATTACHED_PSU) {
    Serial.println("Device Detected: Attached PSU");
  }
}

void initializeStatusLED() {
  status_led.begin();
  status_led.clear();
  status_led.setPixelColor(LED_STATUS_ADDRESS, status_led.Color(10, 10, 10));
  while (!status_led.canShow()) {}
  status_led.show();
}

void updateStatusLED(PSUState commandedSupplyState) {
  status_led.clear();
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
  while (!status_led.canShow()) {}
  status_led.show();
}

void updatePowerState(PSUState commandedSupplyState) {
  Serial.print("Power Supply State: ");
  Serial.println(commandedSupplyState);
  powerStateMachineCommand(commandedSupplyState);
  psuState = commandedSupplyState;
}

void refreshStatusLED() {
  updateStatusLED(psuState);
}
