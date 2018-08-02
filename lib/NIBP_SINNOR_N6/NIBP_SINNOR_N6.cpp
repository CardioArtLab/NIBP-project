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
void NIBP::begin(int8_t rxPin, int8_t txPin) {
  pinMode(rxPin, INPUT_PULLDOWN);
  pinMode(txPin, OUTPUT);
  serial->begin(4800, SERIAL_8N1, rxPin, txPin);
}

bool NIBP::isReady() {
  return _isReady;
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
    if (isFound && byte == '\r') return;
    
    if (state == 0 && byte == 'S') {state = 1; isFound = true;}
    else if (state == 1) {status = byte - '0';state = 2;}
    else if (state == 2 && byte == ';') state = 3;
    else if (state == 3 && byte == 'A') state = 4;
    else if (state == 4) {mode = byte - '0';state = 5;}
    else if (state == 5 && byte == ';') state = 6;
    else if (state == 6 && byte == 'C') state = 7;
    else if (state == 7) {cycleMode = 10*(byte - '0'); state = 8;}
    else if (state == 8) {cycleMode += (byte - '0'); state = 9;}
    else if (state == 9 && byte == ';') state = 10;
    else if (state == 10 && byte == 'M') state = 11;
    else if (state == 11) {message = 10*(byte - '0'); state = 12;}
    else if (state == 12) {message += (byte - '0'); state = 13;}
    else if (state == 13 && byte == ';') state = 14;
    else if (state == 14 && byte == 'P') state = 15;
    else if (state == 15) {systolicPressure = 100*(byte - '0'); state = 16;}
    else if (state == 16) {systolicPressure += 10*(byte - '0'); state = 17;}
    else if (state == 17) {systolicPressure += (byte - '0'); state = 18;}
    else if (state == 18) {diastolicPressure = 100*(byte - '0'); state = 19;}
    else if (state == 19) {diastolicPressure += 10*(byte - '0'); state = 20;}
    else if (state == 20) {diastolicPressure += (byte - '0'); state = 21;}
    else if (state == 21) {meanAtrial = 100*(byte - '0'); state = 22;}
    else if (state == 22) {meanAtrial += 10*(byte - '0'); state = 23;}
    else if (state == 23) {meanAtrial += (byte - '0'); state = 24;}
    else if (state == 24 && byte == ';') state = 25;
    else if (state == 25 && byte == 'R') state = 26;
    else if (state == 26) {pulseRate = 100*(byte - '0'); state = 27;}
    else if (state == 27) {pulseRate += 10*(byte - '0'); state = 28;}
    else if (state == 28) {pulseRate += (byte - '0'); state = 29;}
    else if (state == 29 && byte == ';') state = 30;
    else if (state == 30 && byte == 'T') state = 31;
    else if (state == 31) {usedTime = 1000*(byte - '0'); state = 32;}
    else if (state == 32) {usedTime += 100*(byte - '0'); state = 33;}
    else if (state == 33) {usedTime += 10*(byte - '0'); state = 34;}
    else if (state == 34) {usedTime += (byte - '0'); state = 35;}
    
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
    if (byte == -1) return;
    if (byte == '\r') return;
    
    if (state == 0) {cuffPressure = 100*(byte - '0'); state = 1;}
    else if (state == 1) {cuffPressure += 10*(byte - '0'); state = 2;}
    else if (state == 2) {cuffPressure += (byte - '0'); state = 3;}
    else if (state == 3 && byte == 'C') state = 4;
    else if (state == 4 ) {cuffError = byte - '0'; state = 5;}
    else if (state == 5 && byte == 'S') state = 6;
    else if (state == 6) {cuffStatus = byte - '0'; state = 7;}

    if (byte == 0x02) {state = 0;_isReady = false;}
    if (byte == 0x03) {state = -1;_isReady = true;}
  }
}