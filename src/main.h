#ifndef _MAIN_H_
#define _MAIN_H_

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Preferences.h>
#include <NIBP_SINNOR_N6.h>
#include <NEXTION_HMI.h>

// NodeMCU32s LED_PULLDOWN
// ArayunNode32s LED_PULLUP
// comment below for LED_PULLUP
#define LED_BUILIN 2
#define LED_PULLDOWN
#ifdef LED_PULLDOWN
#define LED_ON HIGH
#define LED_OFF LOW
#else
#define LED_ON LOW
#define LED_OFF HIGH
#endif

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