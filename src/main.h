#ifndef _MAIN_H_
#define _MAIN_H_

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Preferences.h>
#include <NIBP_SINNOR_N6.h>
#include <NEXTION_HMI.h>

#define LED_BUILIN 2
#define NIBP_TX 17
#define NIBP_RX 18
#define HMI_TX 13
#define HMI_RX 14

BluetoothSerial SerialBT;
Preferences preference;

NIBP nibp(&Serial2);
HMI hmi(&Serial1);

void HMITask(void *pvParameters);
void NIBPReaderTask(void *pvParameters);
void BluetoothServerTask(void *pvParameters);
void ATCommandTask(void *pvParameters);
#endif