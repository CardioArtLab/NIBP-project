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
  
  // Main task Controll HMI via UART1
  //xTaskCreatePinnedToCore(HMITask, "HMITask", 2048, NULL, 0, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreate(HMITask, "HMITask", 2048, NULL, 0, NULL);
  //xTaskCreatePinnedToCore(NIBPReaderTask, "NIBPReader", 2048, NULL, 0, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreate(NIBPReaderTask, "NIBPReader", 2048, NULL, 0, NULL);
}

void HMITask(void *pvParameters) 
{
  bool isWorking = false;
  uint8_t tick = 0;
  for (;;) 
  {
    hmi.read();
    if (hmi.getResponseType() == HMI_RESPONSE_TYPE_NUMBER_DATA) {
      if (hmi.getResponseAsNumber() == 0x1b) {
        isWorking = true;
        nibp.start();
      } else if (hmi.getResponseAsNumber() == 0x1c) {
        hmi.renderBPMarker(HMI_BP_TYPE_SYSTOLIC, nibp.systolicPressure);
        hmi.renderBPMarker(HMI_BP_TYPE_DIASTOLIC, nibp.diastolicPressure);
      } else if (hmi.getResponseAsNumber() == 0x00) {
        isWorking = false;
        nibp.stop();
      }
    }
    if (nibp.cuffPressure == 999) {
      isWorking = false;
      nibp.cuffPressure = 0;
      nibp.requestData();
      hmi.setSystolicBP(nibp.systolicPressure);
      hmi.setDiastolicBP(nibp.diastolicPressure);
      hmi.setHeartRate(nibp.pulseRate);
      hmi.sendCmd("isLd.val=2");
    } else if (nibp.isReady() && tick >= 255 && isWorking) {
      tick = 0;
      hmi.setSystolicBP(nibp.cuffPressure);      
    }
    tick++;
  }
  vTaskDelete(NULL);
}

void NIBPReaderTask(void *pvParameters)
{
  for (;;) {
    if (SerialNIBP.available() > 0) {
      nibp.read();
    } else {
      vTaskDelay(30 / portTICK_PERIOD_MS);
    }
  }
  vTaskDelete(NULL);
}