#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebSocketsClient.h>

WebSocketsClient webSocket;
WiFiClient wifiClient;
const char *ssid = "PLANET BIRU";
const char *password = "kodokterbang";
bool connected = false;

String wsPath = "/ws";
String wsTopic = "sms";

const char *user = "kamshory";
const char *pass = "kamshory";
const char *wsHost = "192.168.1.4";
int wsPort = 9000;

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16)
{
  const uint8_t *src = (const uint8_t *) mem;
  Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t) src, len, len);
  for (uint32_t i = 0; i < len; i++)
  {
    if (i % cols == 0)
    {
      Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t) src, i);
    }
    Serial.printf("%02X ", *src);
    src++;
  }
  Serial.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
    case WStype_DISCONNECTED:
      connected = false;
      break;
    case WStype_CONNECTED:
      {
        connected = true;
        // send message to server when Connected
        webSocket.sendTXT("Connected");
      }
      break;
    case WStype_TEXT:
      handleMessage(payload, length);

      break;
    case WStype_BIN:
      hexdump(payload, length);
      break;
    case WStype_PING:
      // pong will be send automatically
      break;
    case WStype_PONG:
      // answer to a ping we send
      break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}

void handleMessage(uint8_t *payload, size_t length)
{
  String message = "";
  int i = 0;
  for (i = 0; i < length; i++)
  {
    message += (char) payload[i];
  }
  DynamicJsonDocument json(1024);
  deserializeJson(json, message);
  const char *command = json["command"];
  const char *responseTopic = json["callback_topic"];
  int callbackDelay = json["callback_delay"];

  String path = wsPath;
  path += "?topic=";
  path += responseTopic;

  WebSocketsClient webSocket2;
  webSocket2.begin(wsHost, wsPort, path.c_str());
 
  webSocket2.setAuthorization(user, pass);
  long lastUpdate = millis();

  while (lastUpdate + callbackDelay >= millis() || WiFi.status() != WL_CONNECTED)
  {
    webSocket2.loop();
    if (webSocket2.sendPing())
    {
      break;
    }
    delay(7);
  }
  i = 0;
  while (!webSocket2.sendTXT(message) && i < 10)
  {
    delay(10);
    webSocket2.loop();
    i++;
  }
}

void setup()
{
  Serial.begin(115200);

  //Serial.setDebugOutput(true);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  String path = wsPath;
  path += "?topic=";
  path += wsTopic;

  webSocket.begin(wsHost, wsPort, path.c_str());
  webSocket.setAuthorization(user, pass);
  webSocket.onEvent(webSocketEvent);
}

void loop()
{
  webSocket.loop();
  delay(2);
}
