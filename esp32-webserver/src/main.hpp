#ifndef MAIN_HPP
#define MAIN_HPP

#include <WiFi.h>

struct WiFiConfig
{
    /* data */
    const char *ssid;
    const char *password;
};

struct ParamConfig
{
    /* data */
    const char *PARAM_INPUT_1;
    const char *PARAM_INPUT_2;
};

String read_uart_data();
String processor(const String &var);
void server_request();

#endif