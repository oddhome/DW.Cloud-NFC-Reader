#include "DWCLOUDWIFI.h"

#define SSID        "DD_WIFI"
#define PASSWORD    "deaw1234567890"
#define HOST_NAME   "192.168.1.38"
#define HOST_PORT   (9999)

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
    wifi.connectTCP(HOST_NAME, HOST_PORT);
    wifi.print("testData");
    
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }
    
    wifi.closeTCP();
    delay(5000);
}
