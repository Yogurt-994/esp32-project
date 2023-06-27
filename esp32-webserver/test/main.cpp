/*
    Serial1串口IO口:
    RX --------- SD2
    TX --------- SD3
*/

#include "WiFi.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"

const char *ssid = "dxxy16-403-1";
const char *password = "1234567890";
// const char *ssid = "dxxy16-402-1";
// const char *password = "dxxy16402";

const char *PARAM_INPUT_1 = "output";
const char *PARAM_INPUT_2 = "state";

String temperatureF = " ";
String temperatureC = " ";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
    <title>ESP Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <style>
        html {
                font-family: Arial; 
                display: inline-block; 
                text-align: center;
                margin: 0px auto;
            }
        h2 {font-size: 3.0rem;}
        p {font-size: 3.0rem;}
        body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
        .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
        .switch input {display: none}
        .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
        .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
        input:checked+.slider {background-color: #b30000}
        input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
        .units { font-size: 1.2rem; }
        .ds-labels{
            font-size: 1.2rem;
            vertical-align:middle;
            padding-bottom: 15px;
        }
    </style>
</head>
<body>
    %BUTTONPLACEHOLDER%

    <p>
        <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
        <span class="ds-labels">Temperature Celsius</span> 
        <span id="temperaturec">%TEMPERATUREC%</span>
        <sup class="units">&deg;C</sup>
    </p>

<script>
function toggleCheckbox(element) {
    var xhr = new XMLHttpRequest();
    if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
    else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
    xhr.send();
}

setInterval(function () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("temperaturec").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "/temperaturec", true);
    xhttp.send();
}, 20000) ;
</script>
</body>
</html>
)rawliteral";

//读取串口数据
char recv_num[100];
String read_uart_data()
{
    unsigned short i = Serial1.available(); //获取串口接收数据个数
    unsigned short count = i;
    unsigned short j;
    char temp;

    if(i!=0)
    {
        j = 0;
        memset(recv_num, 0, sizeof(recv_num));    //清空我们的目标字符串存储区域
        // Serial.print("The amount of data received by the serial port is:");
        // Serial.println(Serial1.available());
        while (i--)
        {
            /* code */
            temp = Serial1.read();
            if(temp=='?')
                break;
            if(temp=='C')
                break;
            recv_num[j] = temp;
            j++;
        }
        Serial1.print(recv_num);
    }
    else
    {
        Serial1.print(recv_num);
    }
    return String(recv_num);
}

// Replaces placeholder with button section in your web page
String processor(const String &var)
{
    if (var == "BUTTONPLACEHOLDER")
    {
        String buttons = "";
        buttons += "<h3>LED0</h3><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"0\"><span class=\"slider\"></span></label>";
        buttons += "<h3>LED1</h3><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"1\"><span class=\"slider\"></span></label>";
        buttons += "<h3>Warning</h3><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\"><span class=\"slider\"></span></label>";
        return buttons;
    }
    if(var == "TEMPERATUREC"){
        return read_uart_data();
    }
    return String();
}

void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    // Print ESP Local IP Address
    Serial.println(WiFi.localIP());

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_html, processor); });

    // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        String inputMessage1;
        String inputMessage2;
        // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
        if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
            inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
            inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
        }
        else {
            inputMessage1 = "No message sent";
            inputMessage2 = "No message sent";
        }
        Serial.print(inputMessage1+inputMessage2);
        Serial1.print(inputMessage1+inputMessage2);

        request->send(200, "text/plain", "OK"); });

    server.on("/temperaturec", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", read_uart_data().c_str());
    });

    // Start server
    server.begin();
}

void loop(){

}

