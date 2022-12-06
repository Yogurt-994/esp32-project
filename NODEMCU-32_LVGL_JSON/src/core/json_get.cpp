#include "json_get.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "../lib/ArduinoJson/ArduinoJson.h"

// wyymusic api: follower
String UID = "1487858279";
String followerUrl = "https://music.163.com/api/v1/user/detail/" + UID; // 粉丝数

// 心知天气api
String apikey = "SCip88i9pJcY90FZ0";
String cityid = "wuhan";
// String weatherUrl = "https://api.seniverse.com/v3/weather/now.json?key=SCip88i9pJcY90FZ0&location=beijing&language=zh-Hans&unit=c";

DynamicJsonDocument docWyy(10000);
DynamicJsonDocument docWeather(1000);

char nickname[20];	//昵称
int follower = 0; // 粉丝数

char location[20];
char weather[15];
char temperature[10];//心知天气温度

//WiFi的初始化和连接
void WiFi_Connect()
{
    WiFi.begin("民大彭于晏", "nideshengri");
	// WiFi.begin("Yogurt", "1234567890qwertyuiop");
    while (WiFi.status() != WL_CONNECTED)
    { //这里是阻塞程序，直到连接成功
        delay(300);
        Serial.print(".");
    }
}

//获取心知天气
void getXinzhiWeather(){
	String weatherUrl = "https://api.seniverse.com/v3/weather/now.json?key=";
	weatherUrl += apikey;
	weatherUrl += "&location=";
	weatherUrl += cityid;
	weatherUrl += "&language=zh-Hans&unit=c";

	HTTPClient http;
	http.begin(weatherUrl); //HTTP begin
	int httpCode = http.GET();

	if (httpCode > 0)
	{
		// httpCode will be negative on error
		// Serial.printf("HTTP Get Code: %d\r\n", httpCode);
		if (httpCode == HTTP_CODE_OK) // 收到正确的内容
		{
			String resBuff = http.getString();

			//	输出示例：{"mid":123456789,"following":226,"whisper":0,"black":0,"follower":867}}
			Serial.println(resBuff);

			//	使用ArduinoJson_6.x版本，具体请移步：https://github.com/bblanchon/ArduinoJson
			deserializeJson(docWeather, resBuff); //开始使用Json解析
			strcpy(location,docWeather["results"][0]["location"]["name"]);
			strcpy(weather,docWeather["results"][0]["now"]["text"]);	
			strcpy(temperature,docWeather["results"][0]["now"]["temperature"]);

			Serial.printf("Location: %s\r\n", location);
			Serial.printf("Weather: %s\r\n", weather);
			Serial.printf("Temperature: %s\r\n", temperature);
		}
	}
	else
	{
		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
	}
	http.end();
}

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
			deserializeJson(docWyy, resBuff); //开始使用Json解析
			// strcpy(nickname,docWyy["profile"][0]["nickename"]);
			// *nickname = docWyy["profile"]["nickename"];
			follower = docWyy["profile"]["followeds"];

			// Serial.printf("Name: %s \r\n", nickname);
			Serial.printf("Followers: %d \r\n", follower);
		}
	}
	else
	{
		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();
}

void xTaskTwoWifi(void *xTask2)
{
	while(1)
	{
		getwyymusicFollower();
		getXinzhiWeather();
		vTaskDelay(2000);
	}
}
