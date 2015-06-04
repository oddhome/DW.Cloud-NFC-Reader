//-----------------------------------
// Send: (NFC PN532)Arduino -->(Serial1)--> ESP8266(NodeMCU) --(TCP80 via Wifi)--> Web Server
// Receive: Web Server -->(TCP80 via Wifi)--> ESP8266(NodeMCU) -->(Serial1)--> Arduino(Nokia5110LCD)
//-----------------------------------

//------------------------------------
// PN532 --> DW.CLOUD
// SDA --> D17 (SDA)
// SCL --> D16 (SCL)
//------------------------------------

//------------------------------------
// Nokia 5110 --> DW.CLOUD
// SCK --> D7
// DIN --> D5
// DC --> D3
// RST --> D2
// CS --> D1
//------------------------------------

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

#include <LCD5110_Basic.h>


PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

int buzzer_pin = 18;

LCD5110 myGLCD(7,5,3,2,1);

extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;


void setup(void) {
  Serial.begin(9600);
  Serial1.begin(9600);
  inputString.reserve(200);

  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, HIGH);
  
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  myGLCD.clrScr();
  
  Serial.println("Init Device!");
  myGLCD.print("Init Device!", CENTER, 0);
  
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  myGLCD.print("NFC Ready", CENTER, 32);
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
    
  Serial.println("Waiting for an ISO14443A card");
}



void loop(void) {
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  if (stringComplete) {
        
    Serial.println(inputString); 
    // clear the string:
    if (inputString.indexOf("success")>0)
    {
      digitalWrite(buzzer_pin, LOW);
      delay(100);
      digitalWrite(buzzer_pin, HIGH);
      
      //Find UserName
      String username_pattern = "\"username\":\"";
      int start_pos = inputString.indexOf(username_pattern) + username_pattern.length();
      int end_pos = inputString.indexOf("\"",start_pos);
      String username = inputString.substring(start_pos,end_pos);
      username.toUpperCase();
      //Find Login Time
      String login_time_pattern = "\"li_time\":\"";
      start_pos = inputString.indexOf(login_time_pattern) + login_time_pattern.length();
      end_pos = inputString.indexOf("\"",start_pos);
      String login_time = inputString.substring(start_pos,end_pos);
      
      
      myGLCD.clrScr();
      myGLCD.setFont(SmallFont);
      myGLCD.print("==Scan Fund==", CENTER, 0);
      myGLCD.print("[==CARD ID==]", CENTER, 8);
      myGLCD.print("", CENTER, 16);
      myGLCD.print(username, CENTER, 24);
      myGLCD.print(login_time, CENTER, 32);
    }
    inputString = "";
    stringComplete = false;
  }
  
  myGLCD.setFont(SmallFont);
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (success) {
    String card_id;
    
    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) 
    {
      Serial.print(" 0x");Serial.print(uid[i], HEX); 
      card_id = card_id + String(uid[i],HEX);
    }
    Serial.println("");
    card_id.toUpperCase();
    
    Serial.println(card_id + "\n");
    Serial1.println(card_id + "\n");
    

    myGLCD.clrScr();
    myGLCD.setFont(SmallFont);
    myGLCD.print("==Scan Fund==", CENTER, 0);
    myGLCD.print("[==CARD ID==]", CENTER, 8);
    myGLCD.print("", CENTER, 16);
    myGLCD.print(card_id, CENTER, 24);
    
    // Wait 1 second before continuing
    delay(1000);
  }
  else
  {
    
    // PN532 probably timed out waiting for a card
    Serial.println("Timed out waiting for a card");
    
    myGLCD.clrScr();
    myGLCD.setFont(SmallFont);
    myGLCD.print("==NNR.NSTDA==", CENTER, 0);
    myGLCD.print("Ready to Scan", CENTER, 8); 
  }
}

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
  asm volatile ("  jmp 0");  
} 

void serialEvent1() {
  while (Serial1.available()) {
    // get the new byte:
    char inChar = (char)Serial1.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '}') {
      stringComplete = true;
    } 
  }
}
