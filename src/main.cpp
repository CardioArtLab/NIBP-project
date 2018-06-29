#include "main.h"

extern "C" void app_main() {
    initArduino();
    
    // Turn on the background LED
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    pinMode(NIBP_TX, OUTPUT);
    pinMode(NIBP_RX, INPUT_PULLDOWN);

    pinMode(HMI_TX, OUTPUT);
    pinMode(HMI_RX, INPUT_PULLDOWN);

    // Debugging serial
    Serial.begin(115200);
    // Random name in form CA-NIBP-<4char>
    // https://www.random.org/cgi-bin/randbyte?nbytes=2&format=h
    // note: CA = CardioArt
    preference.begin("NIBP", true);
    String name = preference.getString("ID", "");
    preference.end();
    if (name.length() == 0)
      SerialBT.begin("CA-NIBP-0000");
    else
      SerialBT.begin("CA-NIBP-" + name);
    printf("START\n");

    // Main task Controll HMI via UART1
    xTaskCreate(HMITask, "HMITask", 2048, NULL, 0, NULL);
    xTaskCreate(NIBPReaderTask, "NIBPReader", 2048, NULL, 0, NULL);
    xTaskCreate(ATCommandTask, "ATCommand", 2048, NULL, 0, NULL);
    //xTaskCreate(BluetoothServerTask, "BTServer", 512, NULL, 0, NULL);
    //HMITask(NULL);
    BluetoothServerTask(NULL);
    for(;;);
}

void HMITask(void *pvParameters) 
{
  Serial.println("HMI Task");
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
    } else if (nibp.isReady() && tick >= 34 && isWorking) {
      tick = 0;
      hmi.setSystolicBP(nibp.cuffPressure);      
    }
    tick++;
    vTaskDelay(30 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void NIBPReaderTask(void *pvParameters)
{
  Serial.println("NIBPReader Task");
  for (;;) {
    if (SerialNIBP.available() > 0) {
      nibp.read();
    } else {
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }
  }
  vTaskDelete(NULL);
}

void BluetoothServerTask(void *pvParameters)
{
  Serial.println("BluetoothServer Task");
  for (;;) {
    if (nibp.isReady() && SerialBT.hasClient()) {
      SerialBT.printf("\002%03d%03d%03d%03d\x03\r", nibp.cuffPressure, nibp.systolicPressure, nibp.diastolicPressure, nibp.pulseRate);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void ATCommandTask(void *pvParameters)
{
  Serial.println("ATCommand Task");
  for(;;) {
    int state = -1;
    bool isReboot = false;
    int8_t b;
    while ((b = Serial.read()) != -1) {
      if (b == 'A') state = 0;
      else if (state == 0 && b =='T') {
        String command = Serial.readStringUntil('\r');
        preference.begin("NIBP");
        if (command.startsWith("ID=")) {
          preference.putString("ID", command.substring(command.indexOf('=')+1));
          isReboot = true;
          //Serial.printf("%s\r\n", command.substring(command.indexOf('=')+1).c_str());
        } else if (command.startsWith("ID")) {
          Serial.printf("%s\r\n", preference.getString("ID", "").c_str());
        }
        preference.end();
        if (isReboot) ESP.restart();
      } else {
        state = -1;
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}