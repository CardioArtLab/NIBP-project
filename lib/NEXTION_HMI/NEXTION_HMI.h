#ifndef _NEXTION_HMI_LIB_
#define _NEXTION_HMI_LIB_

#include "Arduino.h"
#include "pins_arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define HMI_RESPONSE_TYPE_UNDEFINED -1
#define HMI_RESPONSE_TYPE_STARTUP 1
#define HMI_RESPONSE_TYPE_TOUCH_EVENT 0x65
#define HMI_RESPONSE_TYPE_CURRENT_PAGE 0x66
#define HMI_RESPONSE_TYPE_STRING_DATA 0x70
#define HMI_RESPONSE_TYPE_NUMBER_DATA 0x71
#define HMI_BP_TYPE_SYSTOLIC 1
#define HMI_BP_TYPE_DIASTOLIC 2

class HMI
{
private:
  uint8_t type;
  uint8_t buffer[100];
public:
  HMI();
  HMI(HardwareSerial* serial);
  void sendCmd(const char* cmd);
  void renderPicture(uint16_t x, uint16_t y, uint8_t pictureId);
  void renderBPMarker(uint8_t bptype, uint16_t value);
  void read();
  uint8_t getResponseType();
  uint8_t* getResponseAsString();
  uint32_t getResponseAsNumber();
protected:
  HardwareSerial* serial;
};

#endif