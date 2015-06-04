#include "DWCLOUDWIFI.h"

#define SSID        "SSID"
#define PASSWORD    "PassWord"
#define HOST_NAME   "192.168.1.33"
#define HOST_PORT   (1000)

ESP8266 wifi(Serial1, 115200);
uint8_t buffer[128] = {0};

void setup(void)
{
    Serial.begin(9600);
    Serial.print("Start DW.CLOUD\r\n");
    
    wifi.setStation();

    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP Success\r\n");
    } else {
        Serial.print("Join AP Failure\r\n");
    }
}
 
void loop(void)
{
    if (wifi.connectTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("Connect success\r\n");
    } else {
        Serial.print("Connect error\r\n");
    }
    
    wifi.send((uint8_t*)"test", strlen("test"));
    
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }
    
    if (wifi.closeTCP()) {
        Serial.print("close tcp ok\r\n");
    } else {
        Serial.print("close tcp err\r\n");
    }
    delay(5000);
}
