/*One line of code to achieve wifi automatic connection

  Serial.begin(115200) //You must enable the serial port to output debugging information
  wifi_auto.begin()
  
 by SuperDDG 
*/
#include<ESP_AutoWifi.h>

AutoWifi wifi_auto;//初始化一个自动配网对象

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  wifi_auto.begin();//开始自动配网,一直等待wifi连接成功才返回 阻塞

  //wifi_auto.begin(60);//增加一个时间参数 单位为秒，定时在60秒内 wifi无论是否连接成功，都返回 非阻塞
}

void loop() {
  // put your main code here, to run repeatedly:

}