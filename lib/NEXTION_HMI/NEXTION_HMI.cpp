#include "NEXTION_HMI.h"

/*********************************************************
********************** constructors **********************
**********************************************************/

HMI::HMI() {
  serial = &Serial;
  serial->begin(9600, SERIAL_8N1);
}
HMI::HMI(HardwareSerial* serial): serial(serial) {}
void HMI::begin(uint8_t rxPin, uint8_t txPin) {
  pinMode(rxPin, INPUT_PULLDOWN);
  pinMode(txPin, OUTPUT);
  serial->begin(9600, SERIAL_8N1, rxPin, txPin);
}

void HMI::sendCmd(const char* cmd) {
  serial->printf("%s\xff\xff\xff", cmd);
}

void HMI::renderPicture(uint16_t x, uint16_t y, uint8_t pictureId) {
  serial->printf("pic %d,%d,%d\xff\xff\xff", x, y, pictureId);
}

void HMI::renderBPMarker(uint8_t bptype, uint16_t value) {
  uint16_t x = 0;
  if (bptype == HMI_BP_TYPE_SYSTOLIC) {
    if (value <= 50) x = 50;
    else if (value <= 120) x = 50 + (value-50)*54/70;
    else if (value <= 140) x = 104 + (value-120)*3;
    else if (value <= 160) x = 164 + (value-140)*3.2;
    else if (value <= 250) x = 228 + (value-160)*57/90;
    else x = 285;
    renderPicture(x, 120, 9);
  } else if (bptype == HMI_BP_TYPE_DIASTOLIC) {
    if (value <= 30) x = 50;
    else if (value <= 80) x = 50 + (value-30)*54/50;
    else if (value <= 90) x = 104 + (value-80)*6;
    else if (value <= 100) x = 164 + (value-90)*8.4;
    else if (value <= 200) x = 228 + (value-100)*0.57;
    else x = 285;
    renderPicture(x, 182, 10);
  }
}

void HMI::setSystolicBP(uint16_t value) {
  serial->printf("tSys.txt=\"%d\"\xff\xff\xff", value);
}

void HMI::setDiastolicBP(uint16_t value) {
  serial->printf("tDia.txt=\"%d\"\xff\xff\xff", value);
}

void HMI::setHeartRate(uint16_t value) {
  serial->printf("t0.txt=\"%d\"\xff\xff\xff", value);
}

void HMI::read() {
  uint8_t state = 0;
  uint8_t endCount = 0;
  type = HMI_RESPONSE_TYPE_UNDEFINED;
  for(;;)
  {
    int byte = serial->read();
    if (byte == -1) return;
    else if (byte == 0xff) {
      endCount++;
      if (endCount >= 3)
        return;
    }
    printf("%d.", byte);
    if (state == 0) {
      type = byte;
      state = 1;
    }
    if (state == 1 && byte == 0) {
      if (serial->read() == 0 && serial->read() == 0) {
        state = 2;
      } else {
        type = HMI_RESPONSE_TYPE_UNDEFINED;
      }
    } else if (state == 1 && byte == HMI_RESPONSE_TYPE_NUMBER_DATA) {
      buffer[0] = serial->read(); 
      buffer[1] = serial->read();
      buffer[2] = serial->read();
      buffer[3] = serial->read();
      state = 2;
    }
  }
}

uint8_t HMI::getResponseType() {
  return type;
}

uint8_t* HMI::getResponseAsString() {
  return buffer;
}

uint32_t HMI::getResponseAsNumber() {
  return buffer[0] + (buffer[1] << 8) + (buffer[2] << 16) + (buffer[3] << 24);
}