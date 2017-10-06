#include "main.h"

extern "C" void app_main()
{
  initArduino();

  printf("START\n");

  // Turn on the background LED
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  pinMode(NIBP_TX, OUTPUT);
  pinMode(NIBP_RX, INPUT_PULLDOWN);

  pinMode(HMI_TX, OUTPUT);
  pinMode(HMI_RX, INPUT_PULLDOWN);
  
  for (;;) 
  {
    hmi.read();
    if (hmi.getResponseType() == HMI_RESPONSE_TYPE_NUMBER_DATA) {
      if (hmi.getResponseAsNumber() == 0x1b) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        hmi.sendCmd("isLd.val=2");
        hmi.sendCmd("tSys.txt=\"120\"");
        hmi.sendCmd("tDia.txt=\"70\"");
        hmi.sendCmd("t0.txt=\"68\"");        
      } else if (hmi.getResponseAsNumber() == 0x1c) {
        hmi.renderBPMarker(HMI_BP_TYPE_SYSTOLIC, 190);
        hmi.renderBPMarker(HMI_BP_TYPE_SYSTOLIC, 150);
        hmi.renderBPMarker(HMI_BP_TYPE_SYSTOLIC, 160);
        hmi.renderBPMarker(HMI_BP_TYPE_DIASTOLIC, 70);
      } else if (hmi.getResponseAsNumber() == 0x00) {
        printf("Stop");
        //nibo.stop();
      }
    }
  }
  /*
  nibp.start();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  nibp.stop();
  
  while (SerialNIBP.available() > 0) {
    nibp.read();
  }
  printf("\n\n[Cuff] Pressure: %d, Error: %d, Status: %d\n"
    , nibp.cuffPressure
    , nibp.cuffError
    , nibp.cuffStatus
  );

  nibp.requestData();
  
  printf("\n\nSystolic: %d, Diastolic %d, PulseRate %d, time %d\n"
    , nibp.systolicPressure
    , nibp.diastolicPressure
    , nibp.pulseRate
    , nibp.usedTime
  );*/
}