#include "main.h"
#include "display.h"
#include <TFT_eSPI.h>
#include <lvgl.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "../lvgl/examples/lv_examples.h"
#include "core/json_get.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com", 8*3600, 60000);

// LV_IMG_DECLARE(head_photo);

static const uint16_t screenWidth = 128;
static const uint16_t screenHeight = 128;
static lv_disp_draw_buf_t draw_buf; //初始化显示缓冲区
static lv_color_t buf_1[screenWidth * screenHeight];

lv_obj_t *screenMain;
lv_obj_t *labelFans;
lv_obj_t *labelName;
lv_obj_t *labelTime;
lv_obj_t *labelLocation;
lv_obj_t *labelWeather;
lv_obj_t *labelTemperature;
// lv_obj_t *imgHeadPhoto;

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors(&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void xTaskOneLvgl(void *xTask1)
{
    /* TFT init */
    tft.init(ST7735_BLACK);
    tft.setRotation(0);
    /*初始化显示*/
    lv_init();  //初始化lvgl库
    lv_disp_draw_buf_init(&draw_buf, buf_1, NULL, screenWidth * screenHeight); //开启双缓冲

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;  
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;  /*Set your driver function*/
    disp_drv.draw_buf = &draw_buf;      /*Assign the buffer to the display*/
    lv_disp_drv_register(&disp_drv);    //注册显示屏

    screenMain = lv_obj_create(NULL);    //创建一个实际的屏幕对象
    labelName = lv_label_create(screenMain);
    // labelFans = lv_label_create(screenMain);  
    labelTime = lv_label_create(screenMain);
    labelLocation = lv_label_create(screenMain);
    labelWeather = lv_label_create(screenMain);
    labelTemperature = lv_label_create(screenMain);
    // imgHeadPhoto = lv_img_create(screenMain);

    timeClient.begin();

    while(1)
    {
        lv_label_set_long_mode(labelName, LV_LABEL_LONG_WRAP); 
        lv_label_set_text(labelName, "Hello! Yogurt");
        lv_obj_set_size(labelName, 120, 20);
        lv_obj_align(labelName, LV_ALIGN_DEFAULT, 0, 0);
        lv_obj_set_style_text_font(labelName,&lv_font_chinese_14, 0);

        timeClient.update();
        // Serial.println(timeClient.getFormattedTime());
        lv_label_set_long_mode(labelTime, LV_LABEL_LONG_WRAP); 
        lv_label_set_text_fmt(labelTime, "时间: %s",timeClient.getFormattedTime());
        lv_obj_set_size(labelTime, 120, 20);
        lv_obj_align(labelTime, LV_ALIGN_DEFAULT, 0, 20);
        lv_obj_set_style_text_font(labelTime,&lv_font_chinese_14, 0);

        lv_label_set_long_mode(labelLocation, LV_LABEL_LONG_WRAP); 
        lv_label_set_text_fmt(labelLocation, "城市: %s", location);
        lv_obj_set_size(labelLocation, 120, 20);
        lv_obj_align(labelLocation, LV_ALIGN_DEFAULT, 0, 40);
        lv_obj_set_style_text_font(labelLocation,&lv_font_chinese_14, 0);

        lv_label_set_long_mode(labelWeather, LV_LABEL_LONG_WRAP); 
        lv_label_set_text_fmt(labelWeather, "天气: %s",weather);
        lv_obj_set_size(labelWeather, 120, 20);
        lv_obj_align(labelWeather, LV_ALIGN_DEFAULT, 0, 60);
        lv_obj_set_style_text_font(labelWeather,&lv_font_chinese_14, 0);

        lv_label_set_long_mode(labelTemperature, LV_LABEL_LONG_WRAP); 
        lv_label_set_text_fmt(labelTemperature, "气温: %s度",temperature);
        lv_obj_set_size(labelTemperature, 120, 20);
        lv_obj_align(labelTemperature, LV_ALIGN_DEFAULT, 0, 80);
        lv_obj_set_style_text_font(labelTemperature,&lv_font_chinese_14, 0);

        // lv_img_set_src(imgHeadPhoto, &head_photo);
        // lv_obj_align(imgHeadPhoto, LV_ALIGN_CENTER, 64, 64);
        // lv_obj_set_size(imgHeadPhoto, 60, 60);

        lv_scr_load(screenMain); 

        lv_task_handler();
        vTaskDelay(1);
    }
}
