/***************************************************
  Arduino Automatic connection to wifi library targeted at ESP8266
  and ESP32 based boards.

    Arduino自动连接wifi库，针对ESP8266和ESP32基础板。

  This library allows you to configure wifi connections on the board via web pages

    这个库能通过网页配置板子的wifi连接

此库的完成，借鉴了CSDN部分作者文章代码，感谢他们的贡献，在这里将URL贴出来，供大家学习。
    https://blog.csdn.net/joulang/article/details/117760098
    https://blog.csdn.net/weixin_41886734/article/details/117822204
    https://blog.csdn.net/u014091490/article/details/99709431
  Last review/edit by SuperDDG: 2022/01/11
  最近的回顾和编辑:super大大怪
 ****************************************************/
#include<ESP_AutoWifi.h>
#include <../html/Html_code.cpp>

DNSServer _dnsServer;
#ifdef ESP32
	  WebServer _webserver(80);
#elif defined ESP8266
	  ESP8266WebServer _webserver(80);
#endif
extern const char* Root_Html;
extern const char* Submit_Html;

void AutoWifi::begin(void)//一键配网
{
    if(!autoConfig())//检测自动配网是否成功
    {
        wifiWebConfig();//wifi未连接 则开启web 配网
    }
    wait_block_connection();//web客户端处理进程，等待客户端提交wifi信息，连接成功则往下执行 
}

void AutoWifi::begin(uint16_t second)//定时配网
{   
    if(!autoConfig())//检测自动配网是否成功
    {
        wifiWebConfig();//wifi未连接 则开启web 配网
    }
    uint32_t start_time=millis();
    Serial.println("The Countdown Waits For User Web Configuration :"+String(second)+" s");
    while((millis()-start_time)/1000<second)//倒计时
    {
        if(WiFi.status() == WL_CONNECTED){break;}
        wait_nonblock_connection();
    }
    webClose();//定时配网结束 退出循环 关闭DNS服务和web服务
}
void AutoWifi::webBegin(void)
{
    wifiWebConfig();//开启web 配网
    wait_block_connection();//web客户端处理进程，等待客户端提交wifi信息，连接成功则往下执行 
}


void AutoWifi::wifiWebConfig(void)
{
    initSoftAP();
    initDNS();
    initWebServer();
}

bool AutoWifi::autoConfig(void)
{
  WiFi.begin();
  Serial.print("WIFI autoConfig Waiting...");
  for (int i = 0; i < Connection_Time ;i++)
  {
      if (WiFi.status() == WL_CONNECTED)
      {
          Serial.println("WIFI SmartConfig Success");
          Serial.printf("SSID:%s", WiFi.SSID().c_str());
          Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
          Serial.print("LocalIP:");
          Serial.print(WiFi.localIP());
          Serial.print(" ,GateIP:");
          Serial.println(WiFi.gatewayIP());
          return true;
      }
      else
      {
          Serial.print(".");
          delay(1000);
      }
  }
  Serial.println(".");
  Serial.println("WIFI autoConfig Faild!" );
  return false;
}

void AutoWifi::wait_block_connection(void)
{
    while (WiFi.status() != WL_CONNECTED)   //若WIFI已连接 跳出循环 阻塞
    { 
        _dnsServer.processNextRequest();    //检查客户端DNS请求
        _webserver.handleClient();          //检查客户端(浏览器)http请求
    }
    webClose();//关闭DNS服务和web服务
}

void AutoWifi::wait_nonblock_connection(void)//非阻塞
{   
    _dnsServer.processNextRequest();   //检查客户端DNS请求
    _webserver.handleClient();         //检查客户端(浏览器)http请求

}
void AutoWifi::webClose(void)
{
    _dnsServer.stop();    //关闭DNS服务
    _webserver.close();   //关闭WEB服务
}
void AutoWifi::initDNS(void)
{
    //判断将所有地址映射到esp32或8266的ip上是否成功
    if(_dnsServer.start(DNS_PORT, "*", WiFi.softAPIP()))
    { 
        Serial.println("start DnsServer success.");
    }
    else
    {
        Serial.println("start DnsServer failed.");
    }
}

void AutoWifi::initWebServer(void)
{
    //首页响应回调
    _webserver.on("/", HTTP_GET,AutoWifi::handleRoot);
    //设置无法响应的http请求的回调函数
    _webserver.onNotFound(AutoWifi::handleNotFound);
    //连接响应回调
    _webserver.on("/connect",HTTP_POST,AutoWifi::handleRequest);
    //启动WebServer
    _webserver.begin();
}

void AutoWifi::initSoftAP(void)
{
    WiFi.mode(WIFI_AP);//配置为AP模式
    boolean result = WiFi.softAP(AP_SSID, ""); //开启WIFI热点
    if(result)
    {
        IPAddress myIP = WiFi.softAPIP();
        //打印相关信息
        Serial.println("");
        Serial.println("Soft-AP SSID = "+String(AP_SSID));
        Serial.print("Soft-AP IP address = ");
        Serial.println(myIP);
        Serial.println(String("MAC address = ")  + WiFi.softAPmacAddress().c_str());
        Serial.println("waiting ...");
    } 
    else 
    {  //开启热点失败
        Serial.println("WiFiAP Failed");
        delay(3000);
        ESP.restart();  //复位
    }
}
void AutoWifi::handleRequest(void)
{
    static String wifi_ssid;
    static String wifi_pass;
    _webserver.send(200, "text/html", Submit_Html);

    WiFi.softAPdisconnect(true);
    //获取输入的WIFI账户和密码
    wifi_ssid = _webserver.arg("ssid");
    wifi_pass = _webserver.arg("password");

    WiFi.softAPdisconnect();
    Serial.println("WiFi Connect SSID:" + wifi_ssid + "  PASSWORD:" + wifi_pass);

    //设置为STA模式并连接WIFI
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
    uint8_t Connect_time = 0; //用于连接计时，如果长时间连接不成功，复位设备
    while(WiFi.status() != WL_CONNECTED)  //等待WIFI连接成功
    {
        delay(500);
        Serial.print(".");
        Connect_time ++;
        if(Connect_time > 80) {  //长时间连接不上，复位设备
            Serial.println("Connection timeout, checking wifi-info input is correct or try again later!");
            delay(3000);
            ESP.restart();//web配网后连接失败 板子重启          
        }
    }
    Serial.println("");
    Serial.println("WIFI Config Success!");
    Serial.printf("SSID:%s", WiFi.SSID().c_str());
    Serial.print("  LocalIP:");
    Serial.print(WiFi.localIP());
    Serial.println("");
}

void AutoWifi::handleRoot(void)
{
    _webserver.send(200, "text/html",Root_Html);
}

void AutoWifi::handleNotFound(void)
{
    handleRoot();
}