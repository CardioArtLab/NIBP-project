#include "main.h"

extern "C" void app_main() {
    initArduino();
    // Turn on the background LED
    pinMode(LED_BUILIN, OUTPUT);
    pinMode(NIBP_TX, OUTPUT);
    pinMode(HMI_TX, OUTPUT);
 
    digitalWrite(LED_BUILIN, LED_OFF);

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
    Serial.print("START\n");

    // Main task Controll HMI via UART1
    xTaskCreatePinnedToCore(HMITask, "HMITask", 2048, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(NIBPReaderTask, "NIBPReader", 2048, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(ATCommandTask, "ATCommand", 2048, NULL, 0, NULL, 0);
    //xTaskCreate(BluetoothServerTask, "BTServer", 512, NULL, 0, NULL);
    BluetoothServerTask(NULL);
}

void HMITask(void *pvParameters) 
{
  Serial.println("HMI Task");
  hmi.begin(HMI_RX, HMI_TX);
  bool isWorking = false;
  uint8_t tick = 0;
  for (;;) 
  {
    hmi.read();
    if (hmi.getResponseType() == HMI_RESPONSE_TYPE_NUMBER_DATA) {
      if (hmi.getResponseAsNumber() == 0x1b) {
        isWorking = true;
        nibp.start();
        #ifdef IS_DEBUG
        Serial.printf("nibp start\n");
        #endif
      } else if (hmi.getResponseAsNumber() == 0x1c) {
        hmi.renderBPMarker(HMI_BP_TYPE_SYSTOLIC, nibp.systolicPressure);
        hmi.renderBPMarker(HMI_BP_TYPE_DIASTOLIC, nibp.diastolicPressure);
      } else if (hmi.getResponseAsNumber() == 0x00) {
        isWorking = false;
        nibp.stop();
        #ifdef IS_DEBUG
        Serial.printf("nibp stop\n");
        #endif
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
  nibp.begin(NIBP_RX, NIBP_TX);
  for (;;) {
    if (Serial2.available() > 0) {
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
  bool ledLow = false;
  for (;;) {
    if (SerialBT.hasClient()) {
      digitalWrite(LED_BUILIN, LED_ON);
      ledLow = false;
      if (nibp.isReady()) {
        SerialBT.printf("\002%c%c%c%c%c%c%c%c\x03\n", 
          nibp.cuffPressure >> 8,
          nibp.cuffPressure & 0xff,
          nibp.systolicPressure >> 8,
          nibp.systolicPressure & 0xff,
          nibp.diastolicPressure >> 8,
          nibp.diastolicPressure & 0xff,
          nibp.pulseRate >> 8,
          nibp.pulseRate & 0xff);
      }
    } else {
      if (ledLow) {
        digitalWrite(LED_BUILIN, LED_ON);
        ledLow = false;
        vTaskDelay(100 / portTICK_PERIOD_MS);
      } else {
        ledLow = true;
      }
      digitalWrite(LED_BUILIN, LED_OFF);
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
          //Se rial.printf("%s\r\n", command.substring(command.indexOf('=')+1).c_str());
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
  vTaskDelete(NULL);
}