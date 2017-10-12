#ifndef _MAIN_H_
#define _MAIN_H_

//#define _ILI93XX_DEF_FONT_PATH "_fonts/roboto24.c"
//#define _ILI93XX_DEF_FONT_NAME roboto24

#include <Arduino.h>
#include <NIBP_SINNOR_N6.h>
#include <NEXTION_HMI.h>

#define NIBP_TX 17
#define NIBP_RX 16
#define HMI_TX 10
#define HMI_RX 9

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

HardwareSerial SerialNIBP(2);
HardwareSerial SerialHMI(1);

NIBP nibp(&SerialNIBP);
HMI hmi(&SerialHMI);

portMUX_TYPE mux;
void HMITask(void *pvParameters);
void NIBPReaderTask(void *pvParameters);
#endif