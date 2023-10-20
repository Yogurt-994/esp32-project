#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <string.h>
#include "../lib/ArduinoJson/ArduinoJson.h"

// JOSN是一种轻量级的数据交换格式，遵循一下规则：
// 1.并列的数据之间用逗号(,)分隔
// 2.映射用冒号(:)表示
// 3.并列数据的集合(数组)用方括号([])表示
// 4.映射的集合(对象)用大括号({})表示

//	WiFi的初始化和连接
void WiFi_Connect()
{
	WiFi.begin("dxxy16-402-1", "dxxy16402");
	while (WiFi.status() != WL_CONNECTED)
	{ //这里是阻塞程序，直到连接成功
		delay(300);
		Serial.print(".");
	}
}

void setup()
{

	Serial.begin(115200); // open the serial port at 115200 bps;
	delay(100);

	Serial.print("Connecting.. ");

	WiFi_Connect();

	Serial.println("WiFi connected");

	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

// wyymusic api: follower
String UID = "1487858279";
String followerUrl = "https://music.163.com/api/v1/user/detail/" + UID; // 粉丝数

// char name[20] = "0";
long follower = 0; // 粉丝数
// char signature[20] = "0";

DynamicJsonDocument doc(10000);

//	获取粉丝数
void getwyymusicFollower()
{
	HTTPClient http;
	http.begin(followerUrl); //HTTP begin
	int httpCode = http.GET();

	if (httpCode > 0)
	{
		// httpCode will be negative on error
		// Serial.printf("HTTP Get Code: %d\r\n", httpCode);
		if (httpCode == HTTP_CODE_OK) // 收到正确的内容
		{
			String resBuff = http.getString();

			//	输出示例：{"mid":123456789,"following":226,"whisper":0,"black":0,"follower":867}}
			// Serial.println(resBuff);

			//	使用ArduinoJson_6.x版本，具体请移步：https://github.com/bblanchon/ArduinoJson
			deserializeJson(doc, resBuff); //开始使用Json解析
			// *name = doc["profile"]["nickname"];
			follower = doc["profile"]["followeds"];
			// *signature = doc["profile"]["signature"];

			// Serial.printf("name: %s \r\n", name);		//目前不可用
			Serial.printf("Followers: %ld \r\n", follower);
			// Serial.printf("signature: %s \r\n", signature);
			
		}
	}
	else
	{
		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();
}

void loop()
{

	getwyymusicFollower();
	delay(2000);
	delay(2000);
}
