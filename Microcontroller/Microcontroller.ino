#include <Arduino.h>

#include <WiFi.h>
#include <Pixy2SPI_SS.h>

#include <WebSocketsServer.h>

#define BTN_PIN 14
#define BTN_5V_PIN 13
#define BTN_GND_PIN 27

const char* ssid     = "RoBorregos";
const char* password = "**********";
WebSocketsServer webSocket = WebSocketsServer(81);

#define USE_SERIAL Serial
Pixy2SPI_SS pixy;

float last_x = 0.0;
float last_y = 0.0;
int last_btn = 0;

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
  const uint8_t* src = (const uint8_t*) mem;
  USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for(uint32_t i = 0; i < len; i++) {
    if(i % cols == 0) {
      USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }
    USE_SERIAL.printf("%02X ", *src);
    src++;
  }
  USE_SERIAL.printf("\n");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    String payload_str = String((char*) payload);
    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
            if(payload_str=="data"){
              // Send Last Data to Client
              webSocket.sendTXT(num, String(last_x)+","+String(last_y)+","+String(last_btn));
            } else {
              webSocket.sendTXT(num, "0");
            }
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
            hexdump(payload, length);

            // send message to client
            webSocket.sendBIN(num, payload, length);
            break;
    case WStype_ERROR:      
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
    }

}

void setup() {
    // USE_SERIAL.begin(921600);
    USE_SERIAL.begin(115200);

    //Serial.setDebugOutput(true);
    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    // Set pins
    pinMode(BTN_PIN, INPUT);
    pinMode(BTN_5V_PIN, OUTPUT);
    pinMode(BTN_GND_PIN, OUTPUT);
    digitalWrite(BTN_5V_PIN, HIGH);
    digitalWrite(BTN_GND_PIN, LOW);
    pixy.init();

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi..");
    }
  
    // Print ESP Local IP Address
    Serial.println(WiFi.localIP());

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}


void loop() {
    webSocket.loop();
    pixy.ccc.getBlocks();
    if (pixy.ccc.numBlocks)
    {
      Serial.print("Detected ");
      Serial.println(pixy.ccc.numBlocks);
      for (int i=0; i<pixy.ccc.numBlocks; i++)
      {
        Serial.print("  block ");
        Serial.print(i);                                                                  
        Serial.print(": ");
        pixy.ccc.blocks[i].print();
        last_x = pixy.ccc.blocks[i].m_x / 315.0;
        last_y = pixy.ccc.blocks[i].m_y / 207.0;
      }
    }
    if (digitalRead(BTN_PIN) == HIGH) {
      last_btn = 1;
    } else {
      last_btn = 0;
    }
}
