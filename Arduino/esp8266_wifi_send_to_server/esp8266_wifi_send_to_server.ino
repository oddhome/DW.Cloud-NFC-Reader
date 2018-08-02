#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

byte mac[6]; 
bool stringComplete = false;
String inputString = "";
String macaddress = "";

void setup()
{
  Serial.begin(9600);
  Serial.println();

  inputString.reserve(200);

  WiFi.begin("NNR-Devices", "0882614957");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.macAddress(mac);
  macaddress = String(mac[5],HEX) + ":" + String(mac[4],HEX) + ":" + String(mac[3],HEX) + ":" + String(mac[2],HEX) + ":" + String(mac[1],HEX) + ":" + String(mac[0],HEX);
  macaddress.toUpperCase();
  Serial.println("Mac Address: " + macaddress);
}

void loop() {
  if (Serial.available())
  {
    stringComplete = false;
    inputString = "";
    while (Serial.available()) {
      char inChar = (char)Serial.read();
      inputString += inChar;
      if (inChar == '\n') {
        stringComplete = true;
        if (stringComplete) {
          
          inputString.replace("\n","");
          inputString.replace("\r","");
          if (inputString.length()>=5)
          {
            HTTPClient http;
            //Serial.println("Send Data to Server");
            http.begin("http://www.nnr.nstda.or.th/portal/users/save_user_nfc_card/" + inputString + "/" + macaddress);
            int httpCode = http.GET();
            if(httpCode > 0) {
              if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
              }
            }
          }
          
          inputString = "";
          stringComplete = false;

          // Wait 1 second before continuing
          delay(1000);
        }
        //break;
      } 
    }
  }
}

