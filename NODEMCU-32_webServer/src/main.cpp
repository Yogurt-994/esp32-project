// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "DNSServer.h"
#include "SPIFFS.h"

//在端口80上创建AsyncWebServer对象
AsyncWebServer server(80);

void wifiConnect()
{
    const char* wifi_ssid = "dxxy16-402-1";        //SSID
    const char* wifi_password = "dxxy16402";    //密码
    // const char* wifi_ssid = "yogurt";
    // const char* wifi_password = "qwertyuiop";
    // IPAddress AP_local_ip(10,0,10,1);          //IP地址
    // IPAddress AP_gateway(10,0,10,1);           //网关地址
    // IPAddress AP_subnet(255,255,255,0);       //子网掩码
    // const char* AP_ssid = "Yogurt_AP";         //AP SSID
    // const char* AP_password = "12345678";     //AP wifi密码

    //创建AP/STA模式共存
    // WiFi.mode(WIFI_AP_STA);
    // WiFi.softAPConfig(AP_local_ip, AP_gateway, AP_subnet);
    // WiFi.softAP(AP_ssid, AP_password);
    WiFi.begin(wifi_ssid, wifi_password);         //连接WIFI
    Serial.print("Connected");
    //循环，直到连接成功
    while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
    }
    Serial.println();
    IPAddress local_IP = WiFi.localIP();
    Serial.print("WIFI is connected,The local IP address is "); //连接成功提示
    Serial.println(WiFi.localIP());                             //输出本地IP地址
}

//读取串口数据
char recvNum[100];
String readUartData()
{
    unsigned short i = Serial1.available(); //获取串口接收数据个数
    unsigned short count = i;
    unsigned short j;
    char temp;
    // char recvNum[100];

    memset(recvNum, 0, sizeof(recvNum));    //清空我们的目标字符串存储区域
    if(i!=0)
    {
        j = 0;
        Serial.print("The amount of data received by the serial port is:");
        Serial.println(Serial1.available());
        while (i--)
        {
            /* code */
            temp = Serial1.read();
            Serial.print(temp);
            recvNum[j] = temp;
            j++;
        }
        Serial.print("Data:");
        Serial.print(recvNum);
    }
    else
    {
        Serial.println("No data in serial port receiving area!!!");
    }
    return String(recvNum);
}

void spiffsInit()
{
    // 初始化SPIFFS文件系统
    if(!SPIFFS.begin(true)){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
}

void serverRequest()
{
    // 当服务器收到根"/"URL请求时，将index.html文件发送至客户端
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/index.html");
    }); 
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/style.css","text/css");
    });
    server.on("/chart.js", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/chart.js");
    });
    server.on("/jq.js", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/jq.js");
    });
    server.on("/api/update", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send_P(200, "text/plain", readUartData().c_str());
        // request->send_P(200, "text/plain", String("[66,73,97,69,51,57,28,69,36,91,67,99,88,45,73,25,85,31,22,45,53]").c_str());
    });
}

void setup()
{
    // Serial port for debugging purposes
    Serial.begin(115200);
    Serial1.begin(115200);

    //初始化SPIFFS文件系统
    spiffsInit();

    //连接wifi
    wifiConnect();

    //web服务器收到请求并处理
    serverRequest();
    server.begin();
}
 
void loop()
{

}