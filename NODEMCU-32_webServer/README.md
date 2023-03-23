基于Arduino的ESP32物联网服务器实时绘制幅频特性曲线（含远程控制）

# 前言

&emsp; 建立一个异步ESP32 Web Server实时接收来自串口的数据，并在ESP32托管网页上实时绘制曲线，Web客户端可发送指令至ESP32。

# 先前准备

## 1.ESP32开发板+USB供电线

## 2.开发环境Arduino+VScode+PlatformIO

&emsp;开发环境在此不在赘述,google上有相关教程，在此[附一个](https://randomnerdtutorials.com/vs-code-platformio-ide-esp32-esp8266-arduino/)。

## 3.需安装的库

&emsp;[ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) 和 [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) 库。

使用异步Web Server优势：

1.可以处理多个连接

2.当发送请求同时，可以处理其他请求连接

3.处理请求模板更简单

# 代码架构

主要文件在data文件夹和main.cpp内

data文件夹主要是网页代码，main.cpp为我们所编程的功能。

![1679563165053](image/README/1679563165053.png)

## 网页文件

```
<!-- 文档类型声明 -->
<!DOCTYPE html>
<html lang="en">
  <head>
    <!-- 头部信息 -->
    <meta charset="UTF-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <!-- 页面标题 -->
    <title>远程幅频特性曲线</title>
    <!-- 引入样式表 -->
    <link rel="stylesheet" href="./style.css">
    <!-- 引入 jQuery 库 -->
    <script src="./jq.js"></script>
    <!-- 引入 Chart.js 库 -->
    <script src="./chart.js"></script>
  </head>

  <body>
    <!-- 画布 -->
    <div>
      <canvas id="myChart"></canvas>
    </div>

    <!-- 按钮控件1 -->
    <div class="auto-scan">
      <!-- 文字描述 -->
      <div class="info">
        <p>自动扫频</p>
      </div>
      <!-- 按钮控件1-蓝色 -->
      <div class="toggle-btn blue">
        <!-- 复选框 -->
        <input type="checkbox" id="auto_scan_btn" />
        <!-- 开关标记 -->
        <div class="toggle__indicator" for="auto_scan_btn"></div>
      </div>
    </div>

    <!-- 按钮控件2 -->
    <div class="scales_type">
      <!-- 文字描述 -->
      <div class="info">
        <p>纵坐标刻度类型(对数/线性)</p>
      </div>
      <!-- 按钮控件2-绿色 -->
      <div class="toggle-btn green">
        <!-- 复选框 -->
        <input type="checkbox" id="scales_type_btn"/>
        <!-- 开关标记 -->
        <div class="toggle__indicator" for="scales_type_btn"></div>
      </div>
    </div>

  </body>

  <script>
    // 21个点
    const labels = [
      '100', '10k', '20k', '30k', '40k',
      '50k', '60k', '70k', '80k', '90k',
      '100k', '110k', '120k', '130k', '140k',
      '150k', '160k', '170k', '180k', '190k',
      '200k'
    ];

    const data = {
      labels: labels,
      datasets: [{
        label: 'TestData',
        // data: [66, 73, 97, 69, 51, 57, 28, 69, 36, 91, 
        //       67, 99, 88, 45, 73, 25, 85, 31, 22, 45,
        //       53],
        data: [],
        // 关闭点的下方面积填充
        fill: false,
        // 背景色和线色
        backgroundColor: 'rgb(255, 99, 132,0.5)',
        borderColor: 'rgb(255, 99, 132)',
        // 关闭在数据集的其他点上绘制数据集的活动点
        drawActiveElementsOnTop: false,
        // 张力系数
        tension: 0.5
      }]
    };

    const config = {
      type: 'line', //折线图
      data: data,   //数据来源

      options: {
        // x\y坐标轴设置
        scales: {
          x: {
            title: {
              display: true, //始终保持显示
              text: "频率/Hz"
            }
          },
          y: {
            type: 'logarithmic', //对数坐标轴
            title: {
              display: true,
              text: '增益/dB'
            }
          }
        },
    
        /* 数据改变时的速度 */
        animation: {
          duration: 300,
        },
        /* 其他配置 */
        plugins: {
          /* 标题 */
          title: {
            display: true,
            text: '远程幅频率特性曲线',
            font: {
              size: 30,
            },
          }
        }
      },
    };
  </script>

  <script>
    const host = window.location.hostname;
    const myChart = new Chart(
      document.getElementById('myChart'),
      config
    );

    setInterval(() => {
      $.ajax({
        url: `/api/update`,
        type: `GET`,
        success: (data) => {
          console.log(data)
          myChart.data.datasets[0].data = eval(data);
          myChart.update();
        }
      })
    }, 4000);

  /* 纵坐标刻度 */
  $("#scales_type_btn").click((e) => {
    if (e.target.checked) {
      myChart.config._config.options.scales.y.type = 'linear';
      myChart.update();
    } else {
      myChart.config._config.options.scales.y.type = 'logarithmic';
      myChart.update();
    }
  })

  //扫频开关
  var xhr = new XMLHttpRequest();
  $("#auto_scan_btn").click((e) => {
    if (e.target.checked) {
      xhr.open("GET","/update?output="+e.target.checked, true);
    } else {
      xhr.open("GET","/update?output="+e.target.checked, true);
    }
    xhr.send();
  })

  </script>
</html>
```

## Arduino文件

```
// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
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

    // memset(recvNum, 0, sizeof(recvNum));    //清空我们的目标字符串存储区域
    if(i!=0)
    {
        j = 0;
        memset(recvNum, 0, sizeof(recvNum));    //清空我们的目标字符串存储区域
        Serial.print("The amount of data received by the serial port is:");
        Serial.println(Serial1.available());
        while (i--)
        {
            /* code */
            temp = Serial1.read();
            // Serial.print(temp);
            recvNum[j] = temp;
            j++;
        }
        // Serial.print("Data:");
        // Serial.print(recvNum);
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

const char* PARAM_INPUT = "output";
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

    //ESP32服务端将数据传入网页端(客户端)
    server.on("/api/update", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send_P(200, "text/plain", readUartData().c_str());
        // request->send_P(200, "text/plain", String("[66,73,97,69,51,57,28,69,36,91,67,99,88,45,73,25,85,31,22,45,53]").c_str());
    });

    //网页端（客户端）向ESP32服务端发送数据
    server.on("/update",HTTP_GET, [](AsyncWebServerRequest *request)
    {
        String inputMessage;
        if (request->hasParam(PARAM_INPUT)) 
        {
            inputMessage = request->getParam(PARAM_INPUT)->value();
        }
        else 
        {
            inputMessage = "No message sent";
        }
        Serial.println(inputMessage);
        request->send(200, "text/plain", "OK");
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
```

# 实现效果

![1679563631808](image/README/1679563631808.png)

![1679563736786](image/README/1679563736786.png)

# 教程源码链接及其他

[源码链接：https://github.com/Yogurt-994/MyESP32Project/tree/master/NODEMCU-32_webServer]()

[ESP32官网：https://randomnerdtutorials.com/getting-started-with-esp32/https://randomnerdtutorials.com/getting-started-with-esp32/]()
