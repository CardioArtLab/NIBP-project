#ifndef _NIBP_SINNOR_N6_LIB_
#define _NIBP_SINNOR_N6_LIB_

#include "Arduino.h"
#include "pins_arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define NIBP_INIT_INFLAT_100mmHg 0
#define NIBP_INIT_INFLAT_120mmHg 1
#define NIBP_INIT_INFLAT_140mmHg 2
#define NIBP_INIT_INFLAT_160mmHg 3
#define NIBP_INIT_INFLAT_180mmHg 4
#define NIBP_MODE_ADULT 0
#define NIBP_MODE_NEONATE 1

#define NIBP_RESULT_TYPE_END 0
#define NIBP_RESULT_TYPE_PROCESSING 1
#define NIBP_RESULT_ERROR_NORMAL 0 // no errors
#define NIBP_RESULT_ERROR_NEONATAL_CUFF 1 // when in adult mode, but a neonatal cuff is detected
#define NIBP_RESULT_ERROR_ADULT_CUFF 2 // // when in neonatal mode, but a adult cuff is detected
#define NIBP_RESULT_STATUS_MEASUREMENT 3
#define NIBP_RESULT_STATUS_MANOMETER 4
#define NIBP_RESULT_STATUS_WATCHDOG_TEST 5
#define NIBP_RESULT_STATUS_PNEUMATIC_CHECK 7

typedef struct nibp_result_t {
  uint8_t type;
  uint8_t pressure;
  uint8_t error;
  uint8_t status;
} nibp_result;

class NIBP
{
private:
  bool _isReady = false;
public:
  uint16_t cuffPressure;
  uint8_t cuffError;
  uint8_t cuffStatus;
  uint8_t status;
  uint8_t mode;
  uint8_t cycleMode;
  uint8_t message;
  uint16_t systolicPressure;
  uint16_t diastolicPressure;
  uint16_t meanAtrial;
  uint16_t pulseRate;
  uint16_t usedTime;

  NIBP();
  NIBP(HardwareSerial* serial);
  void begin(int8_t rxPin, int8_t txPin);
  void start();
  void stop();
  void beginManometerMode();
  void testPnematic();
  void requestData();
  void setInitInflat(uint8_t init_inflat);
  void setMode(uint8_t mode);
  void read();
  bool isReady();
protected:
  HardwareSerial* serial;
};

#endif