#include "NIBP_SINNOR_N6.h"

/*********************************************************
********************** constructors **********************
**********************************************************/

NIBP::NIBP() {
  serial = &Serial;
  serial->begin(4800, SERIAL_8N1);
}
NIBP::NIBP(HardwareSerial* serial): serial(serial) {
  serial->begin(4800, SERIAL_8N1);
}

void NIBP::start() {
  serial->printf("\00201;;D7\x03\r");
}

void NIBP::stop() {
  serial->printf("X\r");
}

void NIBP::beginManometerMode() {
  serial->printf("\00214;;DB\x03\r");
}

void NIBP::testPnematic() {
  serial->printf("\00217;;DE\x03\r");
}

void NIBP::requestData() {
  serial->printf("\00218;;DF\x03\r");
  uint8_t state = -1;
  bool isFound = false;
  for(;;)
  {
    int byte = serial->read();
    if (byte == -1) continue;
    printf("%d.", byte, byte);
    if (isFound && byte == '\r') return;
    
    if (state == 0 && byte == 'S') {state = 1; isFound = true;}
    else if (state == 1) status = byte - '0';
    else if (state == 1 && byte == ';') state = 2;
    else if (state == 2 && byte == 'A') state = 3;
    else if (state == 3) mode = byte = '0';
    else if (state == 3 && byte == ';') state = 4;
    else if (state == 4 && byte == 'C') state = 5;
    else if (state == 5) {cycleMode = 10*(byte - '0'); state = 6;}
    else if (state == 6) {cycleMode += (byte - '0'); state = 7;}
    else if (state == 7 && byte == ';') state = 8;
    else if (state == 8 && byte == 'M') state = 9;
    else if (state == 9) {message = 10*(byte - '0'); state = 10;}
    else if (state == 10) {message += (byte - '0'); state = 11;}
    else if (state == 11 && byte == ';') state = 12;
    else if (state == 12 && byte == 'P') state = 13;
    else if (state == 13) {systolicPressure = 100*(byte - '0'); state = 14;}
    else if (state == 14) {systolicPressure += 10*(byte - '0'); state = 15;}
    else if (state == 15) {systolicPressure += (byte - '0'); state = 16;}
    else if (state == 16) {diastolicPressure = 100*(byte - '0'); state = 17;}
    else if (state == 17) {diastolicPressure += 10*(byte - '0'); state = 18;}
    else if (state == 18) {diastolicPressure += (byte - '0'); state = 19;}
    else if (state == 19) {meanAtrial = 100*(byte - '0'); state = 20;}
    else if (state == 20) {meanAtrial += 10*(byte - '0'); state = 21;}
    else if (state == 21) {meanAtrial += (byte - '0'); state = 22;}
    else if (state == 22 && byte == ';') state = 23;
    else if (state == 23 && byte == 'R') state = 24;
    else if (state == 24) {pulseRate = 100*(byte - '0'); state = 25;}
    else if (state == 25) {pulseRate += 10*(byte - '0'); state = 26;}
    else if (state == 26) {pulseRate += (byte - '0'); state = 27;}
    else if (state == 27 && byte == ';') state = 28;
    else if (state == 28 && byte == 'T') state = 29;
    else if (state == 29) {usedTime = 1000*(byte - '0'); state = 30;}
    else if (state == 30) {usedTime += 100*(byte - '0'); state = 31;}
    else if (state == 31) {usedTime += 10*(byte - '0'); state = 32;}
    else if (state == 32) {usedTime += (byte - '0'); state = 33;}
    
    if (byte == 0x02) state = 0;
    if (byte == 0x03) state = -1;
  }
}

void NIBP::setInitInflat(uint8_t init_inflat) {
  if (init_inflat == NIBP_INIT_INFLAT_100mmHg)
    serial->printf("\00219;;E0\x03\r");
  else if (init_inflat == NIBP_INIT_INFLAT_120mmHg)
    serial->printf("\00220;;D8\x03\r");
  else if (init_inflat == NIBP_INIT_INFLAT_140mmHg)
    serial->printf("\00221;;D9\x03\r");
  else if (init_inflat == NIBP_INIT_INFLAT_160mmHg)
    serial->printf("\00222;;DA\x03\r");
  else if (init_inflat == NIBP_INIT_INFLAT_180mmHg)
    serial->printf("\00223;;DB\x03\r");
  while(serial->available() > 0) {
    serial->read();
  }
}

void NIBP::setMode(uint8_t mode) {
  if (mode == NIBP_MODE_ADULT)
    serial->printf("\00224;;DC\x03\r");
  else if (mode == NIBP_MODE_NEONATE)
    serial->printf("\00225;;DD\x03\r");
  while(serial->available() > 0) {
    serial->read();
  }
}

void NIBP::read() {
  uint8_t state = -1;
  for(;;)
  {
    int byte = serial->read();
    if (byte == -1) continue;
    if (byte == '\r') return;
    
    if (state == 0) {cuffPressure = 100*(byte - '0'); state = 1;}
    else if (state == 1) {cuffPressure += 10*(byte - '0'); state = 2;}
    else if (state == 2) {cuffPressure += (byte - '0'); state = 3;}
    else if (state == 3 && byte == 'C') state = 4;
    else if (state == 4 ) {cuffError = byte - '0'; state = 5;}
    else if (state == 5 && byte == 'S') state = 6;
    else if (state == 6) {cuffStatus = byte - '0'; state = 7;}

    if (byte == 0x02) state = 0;
    if (byte == 0x03) state = -1;
  }
}