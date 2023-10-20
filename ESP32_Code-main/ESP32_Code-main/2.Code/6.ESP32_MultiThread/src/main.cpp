/*
//  多线程基于FreeRTOS，可以多个任务并行处理；
//  ESP32具有两个32位Tensilica Xtensa LX6微处理器；
//  实际上我们用Arduino进行编程时只使用到了第一个核（大核），第0核并没有使用
//  多线程可以指定在那个核运行；
 */

#include <Arduino.h>
#define USE_MULTCORE 1

void xTaskOne(void *xTask1)
{
    while (1)
    {
        Serial.printf("Task1 \r\n");
        delay(500);
    }
}

void xTaskTwo(void *xTask2)
{
    while (1)
    {
        Serial.printf("Task2 \r\n");
        delay(1000);
    }
}

void setup()
{
    Serial.begin(115200);
    delay(10);

#if !USE_MULTCORE

    xTaskCreate(
        xTaskOne,  /* Task function. */
        "TaskOne", /* String with name of task. */
        4096,      /* Stack size in bytes. */
        NULL,      /* Parameter passed as input of the task */
        1,         /* Priority of the task.(configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.) */
        NULL);     /* Task handle. */

    xTaskCreate(
        xTaskTwo,  /* Task function. */
        "TaskTwo", /* String with name of task. */
        4096,      /* Stack size in bytes. */
        NULL,      /* Parameter passed as input of the task */
        2,         /* Priority of the task.(configMAX_PRIORITIES - 1 being the highest, and 0 being the lowest.) */
        NULL);     /* Task handle. */

#else

    //最后一个参数至关重要，决定这个任务创建在哪个核上.PRO_CPU 为 0, APP_CPU 为 1,或者 tskNO_AFFINITY 允许任务在两者上运行.
    xTaskCreatePinnedToCore(xTaskOne, "TaskOne", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(xTaskTwo, "TaskTwo", 4096, NULL, 2, NULL, 1);

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
