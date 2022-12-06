/*
    WiFi.begin()

    do somethong····

    if(WiFi.status() != WL_CONNECTED){
        wifi_auto.webBegin()
 }
  
by SuperDDG
*/
#include <ESP_AutoWifi.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI(); 
TFT_eSprite clk = TFT_eSprite(&tft);

AutoWifi wifi_auto;//初始化一个自动配网对象

byte loadNum = 6;
void loading(byte delayTime)//启动动画
{
    clk.setColorDepth(8);
    
    clk.createSprite(200, 50);
    clk.fillSprite(0x0000);

    clk.drawRoundRect(0,0,200,16,8,0xFFFF);
    clk.fillRoundRect(3,3,loadNum,10,5,0xFFFF);
    clk.setTextDatum(CC_DATUM);
    clk.setTextColor(TFT_GREEN, 0x0000); 
    clk.drawString("Connecting to WiFi",100,40,2);
    clk.pushSprite(20,67);//20 110
    clk.deleteSprite();
    loadNum += 1;
    if(loadNum>=195){
      loadNum = 195;
    }
    delay(delayTime);
}

void setup() 
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    tft.begin();
    tft.initDMA();
    tft.setRotation(0);//横屏
    tft.fillScreen(TFT_BLACK);
    WiFi.begin(); //连接保存的wifi
    delay(1000); //等待1秒
    while (WiFi.status() != WL_CONNECTED) 
    {
        for(byte n=0;n<10;n++){ //每500毫秒检测一次状态
            loading(50);
        }
        if(loadNum>=195){//进度条走完，wifi没连接成功 则直接进入web配网
            tft.fillScreen(TFT_BLACK);//黑色
            tft.setTextColor(TFT_BLACK,TFT_RED);
            tft.drawString(" WIFI CONNECT FAILED ",20,10,2);
            tft.setTextColor(TFT_BLACK,TFT_WHITE);
            tft.drawString(" Wifi Web configuration ",20,50,2);
            tft.drawString(" Please Open Ip In Browser ",20,70,2);
            tft.drawString(" AP-SSID: "+String(AP_SSID),20,90,2);
            tft.drawString(" WebServer IP: "+String(WiFi.softAPIP().toString()),20,110,2);
            wifi_auto.webBegin();//web配网
            break;
        }
    }
    while(loadNum < 195){ //如果wifi连接成功让动画跑完
        loading(3);
    }
    if (WiFi.status() == WL_CONNECTED) //判断wifi是否连接成功
    { 
        Serial.println("wifi is connected!");
        Serial.print("SSID: ");
        Serial.println(WiFi.SSID());
        IPAddress ip = WiFi.localIP();
        Serial.print("IP Address: ");
        Serial.println(ip);
        tft.fillScreen(TFT_BLACK);//黑色
        tft.setTextColor(TFT_BLACK,TFT_WHITE);
        tft.drawString("Wifi Have Connected To "+String(WiFi.SSID()),20,20,2);

        tft.drawString("IP: "+ip.toString(),20,40,2);
    }
      //
}

void loop() {
  // put your main code here, to run repeatedly:


}