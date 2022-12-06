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
#ifndef _ESP_AUTOWIFI_H_
#define _ESP_AUTOWIFI_H_

#include <Arduino.h>
#if defined ESP32
    #include <WiFi.h>
    #include <WebServer.h>
    #define AP_SSID   "ESP32_Config" //热点名称
#elif defined ESP8266
    #include <ESP8266WiFi.h>
    #include <ESP8266WebServer.h>
    #define AP_SSID   "ESP8266_Config" //热点名称
#endif

#include <DNSServer.h>
#define Connection_Time 15  //在此时间内尝试连接保存的wifi 超过此时间则开启WEB配网 时间单位（秒）

class AutoWifi;
class AutoWifi 
{
    public:
        /* 如果你想要快捷连接wifi，只需要调用begin()，此函数是阻塞的，直到wifi连接成功才会返回 */
        void begin(void);                   //一键自动配网 阻塞

        /*如果你想在固定时间内连接wifi，只需调用 begin(second),second为一个单位为秒的时间参数，
        超过此时间 即使wifi没有连接成功，也会跳出程序向下执行*/
        void begin(uint16_t second);        //定时配网 单位s

        /*如果你想直接进入web页面进行配网，则调用 webBegin(),此函数是阻塞的 会一直等待用户提交wifi信息直到连接成功后才返回，
        提交的wifi信息连接失败，板子会重启重新配网*/
        void webBegin(void);                //直接进入web配网

        void wifiWebConfig(void);           //配置板子的AP，注册web网页回调函数等事务

        //Connection_Time时间内 连接上一次保存的wifi，连接失败返回False
        bool autoConfig(void);              //自动配网

        void webClose(void);                //关闭web和DNS服务
        void wait_block_connection(void);   //等待客户端请求事务 阻塞等待
        void wait_nonblock_connection(void);//非阻塞式等待，此函数必须包含在while()循环结构中，才能持续监测用户是否提交数据
    private:
        static void handleRoot(void);       //web主页响应回调函数
        static void handleNotFound(void);   //无法响应的http请求的回调函数
        static void handleRequest(void);    //提交wifi信息响应回调函数
        void initDNS(void);                 //初始化DNS配置 
        void initWebServer(void);           //初始化web服务器
        void initSoftAP(void);              //初始化AP信息
        const byte DNS_PORT = 53;           //设置DNS端口号
};

#endif