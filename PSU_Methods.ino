#define PSU_I2C_ADDRESS 0x56

void readAndDisplayPSU() {
  int address = PSU_I2C_ADDRESS;
  Serial.println("Reading PSU registers...");

  for (uint8_t reg = 0; reg < 254; reg++) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    uint8_t status = Wire.endTransmission();

    if (status == 0) {
      Wire.requestFrom(address, 1);
      if (Wire.available()) {
        uint8_t value = Wire.read();
        if (value != 0xFF) {
          Serial.print("Register 0x");
          Serial.print(reg, HEX);
          Serial.print(": 0x");
          Serial.println(value, HEX);
        }
      }
    }
  }

  Serial.println("Done reading PSU registers.");
}

void readInputVoltage() {
  int address = PSU_I2C_ADDRESS;
  Wire.beginTransmission(address);
  Wire.write(0x88);
  uint8_t status = Wire.endTransmission();

  if (status == 0) {
    Wire.requestFrom(address, 2);
    uint8_t HIGH = 0;
    uint8_t LOW = 0;
    uint8_t byteNum = 0;
    while (Wire.available()) {
      uint8_t value = Wire.read();
      if (byteNum == 1){
        HIGH = value;
      }
      if (byteNum == 0){
        LOW = value;
      }
      byteNum++;
    }
    float voltage = ((float)((HIGH << 8) | LOW))/92.5;
    Serial.print("Input Voltage Value: ");
    Serial.print(voltage);
    Serial.println("V");
  }
}