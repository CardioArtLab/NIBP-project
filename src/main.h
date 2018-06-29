#ifndef _MAIN_H_
#define _MAIN_H_

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Preferences.h>
#include <NIBP_SINNOR_N6.h>
#include <NEXTION_HMI.h>

#define NIBP_TX 17
#define NIBP_RX 18
#define HMI_TX 10
#define HMI_RX 9

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

HardwareSerial SerialNIBP(2);
HardwareSerial SerialHMI(1);
BluetoothSerial SerialBT;
Preferences preference;

NIBP nibp(&SerialNIBP, NIBP_RX, NIBP_TX);
HMI hmi(&SerialHMI);

void HMITask(void *pvParameters);
void NIBPReaderTask(void *pvParameters);
void BluetoothServerTask(void *pvParameters);
void ATCommandTask(void *pvParameters);
#endif