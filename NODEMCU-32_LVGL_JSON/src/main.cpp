#include <main.h>
#include <WiFi.h>
#include <NTPClient.h>
#include "core/display.h"
#include "core/json_get.h"

void setup(void) 
{
    Serial.begin(115200);

    /* WIFI init */
    Serial.print("Connecting.. ");
    WiFi_Connect();
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
#if !MULTCOREUSE
    xTaskCreate(
        xTaskOneLvgl,  /* Task function. */
        "TaskOne", /* String with name of task. */
        4096,      /* Stack size in bytes. */
        NULL,      /* Parameter passed as input of the task */
        2,         /* Priority of the task.(configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.) */
        NULL);     /* Task handle. */
    xTaskCreate(
        xTaskTwoWifi,  /* Task function. */
        "TaskTwo", /* String with name of task. */
        4096,      /* Stack size in bytes. */
        NULL,      /* Parameter passed as input of the task */
        1,         /* Priority of the task.(configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.) */
        NULL);     /* Task handle. */
#else
    //最后一个参数至关重要，决定这个任务创建在哪个核上.PRO_CPU 为 0, APP_CPU 为 1,或者 tskNO_AFFINITY 允许任务在两者上运行.
    xTaskCreatePinnedToCore(xTaskOneLvgl, "TaskOne", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(xTaskTwoWifi, "TaskTwo", 4096, NULL, 2, NULL, 0);
#endif
}

void loop()
{
    while (1)
    {
        Serial.printf("XTask is running\r\n");
        delay(1000);
    }
}
