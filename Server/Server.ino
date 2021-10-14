#include <PubSubClient.h>
#include <SPI.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

char *ssid              = "PLANET BIRU";
char *password          = "burungperkutut";

char *ssid2             = "OTP-Mini";
char *password2         = "OTP-Mini";

int eepromDataLength50  = 50;
int eepromDataLength10  = 10;

int offsetSSID1         = 0;
int offsetSSID2         = 50;
int offsetSSIDPassword1 = 100;
int offsetSSIDPassword2 = 150;
int offsetMQTTHost      = 200;
int offsetMQTTPort      = 250;
int offsetclient        = 260;
int offsetMQTTUsername  = 310;
int offsetMQTTPassword  = 360;
int offsetMQTTTopic     = 410;
int offsetMQTTQOS       = 460;
int offsetEnable        = 470;

WebServer server(80);

// Define two tasks for Task1 &Task2
void Task1(void *pvParameters);
void Task2(void *pvParameters);

void handleRoot()
{
  String response = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>OTP-Mini</title><link rel=\"stylesheet\" href=\"style.css\"><script src=\"ajax.js\"></script></head><body><div class=\"all\"><h3>OTP-Mini</h3><div class=\"form-item\"><div class=\"row\"><div class=\"column\"><input class=\"btn btn-success\" type=\"button\" name=\"save\" id=\"sub\" value=\"Subscribtion\" onclick=\"window.location='subscribtion-configuration.html';\"></div><div class=\"column\"><input class=\"btn btn-success\" type=\"button\" name=\"save\" id=\"ap\" value=\"Access Point\" onclick=\"window.location='ap-configuration.html';\"></div></div></div></div></body></html>";
  server.sendHeader("Cache-Control", "public, max-age=2678400");
  server.send(200, "text/html", response);
}

void handleAP()
{
  String response = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Access Point Configuration</title><link rel=\"stylesheet\" href=\"style.css\"><script src=\"ajax.js\"></script></head><body><div class=\"all\"><h3>Access Point Configuration</h3><form action=\"\" method=\"post\"><div class=\"form-item\"><div class=\"form-label\">SSID</div><div class=\"form-input\"><input type=\"text\" name=\"ssid_name\" id=\"ssid_name\"></div></div><div class=\"form-item\"><div class=\"form-label\">Password</div><div class=\"form-input\"><input type=\"password\" name=\"ssid_password\" id=\"ssid_password\"></div></div><div class=\"form-item\"><div class=\"row\"><div class=\"column\"><input class=\"btn btn-success\" type=\"button\" name=\"save\" id=\"save\" value=\"Save\" onclick=\"return saveAPData();\"></div><div class=\"column\"><input class=\"btn btn-danger\" type=\"button\" name=\"save\" id=\"home\" value=\"Home\" onclick=\"window.location='index.html';\"></div></div></div></form></div></body></html>";
  server.sendHeader("Cache-Control", "public, max-age=2678400");
  server.send(200, "text/html", response);
}

void handleSub()
{

  String response = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Subscribtion Configuration</title><link rel=\"stylesheet\" href=\"style.css\"><script src=\"ajax.js\"></script></head><body><div class=\"all\"><h3>Subscribtion Configuration</h3><form action=\"\" method=\"post\"><div class=\"form-item\"><div class=\"form-label\">SSID</div><div class=\"form-input\"><input type=\"text\" name=\"ssid_name\" id=\"ssid_name\"></div></div><div class=\"form-item\"><div class=\"form-label\">Password</div><div class=\"form-input\"><input type=\"password\" name=\"ssid_password\" id=\"ssid_password\"></div></div><div class=\"form-item\"><div class=\"form-label\">MQTT Host</div><div class=\"form-input\"><input type=\"text\" name=\"mqtt_host\" id=\"mqtt_host\"></div></div><div class=\"form-item\"><div class=\"form-label\">MQTT Port</div><div class=\"form-input\"><input type=\"text\" name=\"mqtt_port\" id=\"mqtt_port\"></div></div><div class=\"form-item\"><div class=\"form-label\">MQTT Client ID</div><div class=\"form-input\"><input type=\"text\" name=\"mqtt_client\" id=\"mqtt_client\"></div></div><div class=\"form-item\"><div class=\"form-label\">MQTT Username</div><div class=\"form-input\"><input type=\"text\" name=\"mqtt_username\" id=\"mqtt_username\"></div></div><div class=\"form-item\"><div class=\"form-label\">MQTT Password</div><div class=\"form-input\"><input type=\"password\" name=\"mqtt_password\" id=\"mqtt_password\"></div></div><div class=\"form-item\"><div class=\"form-label\">Topic</div><div class=\"form-input\"><input type=\"text\" name=\"mqtt_topic\" id=\"mqtt_topic\"></div></div><div class=\"form-item\"><div class=\"form-label\">MQTT QoS</div><div class=\"form-input\"><input type=\"number\" name=\"mqtt_qos\" id=\"mqtt_qos\"></div></div><div class=\"form-item\"><div class=\"form-label\">Enable</div><div class=\"form-input\"><select name=\"enable\" id=\"enable\"><option value=\"0\">No</option><option value=\"1\">Yes</option></select></div></div><div class=\"form-item\"><div class=\"row\"><div class=\"column\"><input class=\"btn btn-success\" type=\"button\" name=\"save\" id=\"save\" value=\"Save\" onclick=\"return saveSubData();\"></div><div class=\"column\"><input class=\"btn btn-danger\" type=\"button\" name=\"save\" id=\"home\" value=\"Home\" onclick=\"window.location='index.html';\"></div></div></div></form></div></body></html>";
  server.sendHeader("Cache-Control", "public, max-age=2678400");
  server.send(200, "text/html", response);
}

void handleStyle()
{
  String response = "body{margin:0;padding:0;position:relative;font-family:Verdana,Geneva,Tahoma,sans-serif;font-size:13px;color:#555}h3{margin:4px 0}.all{width:400px;max-width:100%;margin:auto;padding:20px;box-sizing:border-box}.form-label{padding:5px 0}.form-input{position:relative}.form-input input[type=\"text\"],.form-input input[type=\"number\"],.form-input input[type=\"password\"],.form-input select{width:100%;box-sizing:border-box;padding:6px 10px;border-radius:3px;border:1px solid #CCC;background-color:#FFF;margin-bottom:2px}.form-input input[type=\"text\"]:focus,.form-input input[type=\"number\"]:focus,.form-input input[type=\"password\"]:focus,.form-input select:focus{outline:none}.btn{width:100%;box-sizing:border-box;padding:6px 10px;border-radius:3px;border:1px solid #bdbcbc;background-color:#c7c6c6;color:#333;margin:8px 0}.btn-success{color:#FFF;border:1px solid #46992d;background-color:#419129}.btn-primary{color:#FFF;border:1px solid #2d5899;background-color:#3464ac}.btn-warning{color:#212529;border:1px solid #ffc107;background-color:#ffc107}.btn-danger{color:#FFF;border:1px solid #dc3545;background-color:#dc3545}.row{display:flex;gap:10px}.column{flex:50%;justify-content:space-between}";
  server.sendHeader("Cache-Control", "public, max-age=2678400");
  server.send(200, "text/css", response);
}

void handleScript()
{
  String response = "var ajax={};function saveSubData(){var e=document.querySelector(\"#ssid_name\").value,t=document.querySelector(\"#ssid_password\").value,o=document.querySelector(\"#mqtt_host\").value,n=document.querySelector(\"#mqtt_port\").value,a=document.querySelector(\"#mqtt_client\").value,r=document.querySelector(\"#mqtt_username\").value,u=document.querySelector(\"#mqtt_password\").value,s=document.querySelector(\"#mqtt_topic\").value,c=document.querySelector(\"#mqtt_qos\").value,d=document.querySelector(\"#enable\").value;return ajax.post(\"save-subscribtion\",{action:\"save-subscribtion\",ssid_name:e,ssid_password:t,mqtt_host:o,mqtt_port:n,mqtt_client:a,mqtt_username:r,mqtt_password:u,mqtt_topic:s,mqtt_qos:c,enable:d},function(e){},!0),!1}function loadSubData(){ajax.get(\"subscribtion-configuration.json\",{},function(e){e=JSON.parse(e);document.querySelector(\"#ssid_name\").value=e.ssid_name,document.querySelector(\"#ssid_password\").value=e.ssid_password,document.querySelector(\"#mqtt_host\").value=e.mqtt_host,document.querySelector(\"#mqtt_port\").value=e.mqtt_port,document.querySelector(\"#mqtt_client\").value=e.mqtt_client,document.querySelector(\"#mqtt_username\").value=e.mqtt_username,document.querySelector(\"#mqtt_password\").value=e.mqtt_password,document.querySelector(\"#mqtt_topic\").value=e.mqtt_topic,document.querySelector(\"#mqtt_qos\").value=e.mqtt_qos,document.querySelector(\"#enable\").value=e.enable},!0)}function loadAPData(){ajax.get(\"ap-configuration.json\",{},function(e){e=JSON.parse(e);document.querySelector(\"#ssid_name\").value=e.ssid_name,document.querySelector(\"#ssid_password\").value=e.ssid_password},!0)}function saveAPData(){var e=document.querySelector(\"#ssid_name\").value,t=document.querySelector(\"#ssid_password\").value;return ajax.post(\"save-ap\",{action:\"save-ap\",ssid_name:e,ssid_password:t},function(e){},!0),!1}ajax.create=function(){if(\"undefined\"!=typeof XMLHttpRequest)return new XMLHttpRequest;for(var e,t=[\"MSXML2.XmlHttp.6.0\",\"MSXML2.XmlHttp.5.0\",\"MSXML2.XmlHttp.4.0\",\"MSXML2.XmlHttp.3.0\",\"MSXML2.XmlHttp.2.0\",\"Microsoft.XmlHttp\"],o=0;o < t.length;o++)try{e=new ActiveXObject(t[o]);break}catch(e){}return e},ajax.send=function(e,t,o,n,a){void 0===a&&(a=!0);var r=ajax.create();r.open(o,e,a),r.onreadystatechange=function(){4==r.readyState&&t(r.responseText)},\"POST\"==o&&r.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\"),r.send(n)},ajax.get=function(e,t,o,n){var a,r=[];for(a in t)t.hasOwnProperty(a)&&r.push(encodeURIComponent(a)+\"=\"+encodeURIComponent(t[a]));ajax.send(e+(r.length?\"?\"+r.join(\"&\"):\"\"),o,\"GET\",null,n)},ajax.post=function(e,t,o,n){var a,r=[];for(a in t)t.hasOwnProperty(a)&&r.push(encodeURIComponent(a)+\"=\"+encodeURIComponent(t[a]));ajax.send(e,o,\"POST\",r.join(\"&\"),n)},window.onload=function(){var e=window.location.toString();-1<e.indexOf(\"ap-configuration.html\")&&loadAPData(),-1<e.indexOf(\"subscribtion-configuration.html\")&&loadSubData()};";
  server.sendHeader("Cache-Control", "public, max-age=2678400");
  server.send(200, "text/javascript", response);
}

void getAPData()
{
  String savedSSID = readDataString(offsetSSID1, eepromDataLength50);
  String savedSSIDPassword = readDataString(offsetSSIDPassword1, eepromDataLength50);
  String response = "";
  response += "{\"ssid_name\":\"";
  response += savedSSID;
  response += "\", \"ssid_password\":\"";
  response += savedSSIDPassword;
  response += "\"}";
  server.send(200, "application/json", response);
}

void saveAPData()
{
  if (server.method() == HTTP_POST)
  {
    String savedSSID = server.arg("ssid_name");
    writeData(offsetSSID1, eepromDataLength50, savedSSID);
    String savedSSIDPassword = server.arg("ssid_password");
    writeData(offsetSSIDPassword1, eepromDataLength50, savedSSIDPassword);
  }
  String message = "{\"status\":\"OK\"}";
  server.send(200, "application/json", message);
}

void resetAP()
{
  String savedSSID = "OTP-Mini";
  writeData(offsetSSID1, eepromDataLength50, savedSSID);
  String savedSSIDPassword = "OTP-Mini";
  writeData(offsetSSIDPassword1, eepromDataLength50, savedSSIDPassword);
}

void resetSTA()
{
  String savedSSID = "PLANET BIRU";
  String savedSSIDPassword = "kodokterbang";
  writeData(offsetSSID2, eepromDataLength50, savedSSID);
  writeData(offsetSSIDPassword2, eepromDataLength50, savedSSIDPassword);
}

void getSubData()
{
  String savedSSID = readDataString(offsetSSID2, eepromDataLength50);
  String savedSSIDPassword = readDataString(offsetSSIDPassword2, eepromDataLength50);
  String savedMQTTHost = readDataString(offsetMQTTHost, eepromDataLength50);
  String savedMQTTPort = readDataString(offsetMQTTPort, eepromDataLength50);
  String savedclient = readDataString(offsetclient, eepromDataLength50);
  String savedMQTTUsername = readDataString(offsetMQTTUsername, eepromDataLength50);
  String savedMQTTPassword = readDataString(offsetMQTTPassword, eepromDataLength50);
  String savedMQTTTopic = readDataString(offsetMQTTTopic, eepromDataLength50);
  String savedMQTTQOS = readDataString(offsetMQTTQOS, eepromDataLength10);
  String savedEnable = readDataString(offsetEnable, eepromDataLength10);

  String response = "";

  response += "{\"ssid_name\":\"";
  response += savedSSID;

  response += "\", \"ssid_password\":\"";
  response += savedSSIDPassword;

  response += "\", \"mqtt_host\":\"";
  response += savedMQTTHost;

  response += "\", \"mqtt_port\":\"";
  response += savedMQTTPort;

  response += "\", \"mqtt_client\":\"";
  response += savedclient;

  response += "\", \"mqtt_username\":\"";
  response += savedMQTTUsername;

  response += "\", \"mqtt_password\":\"";
  response += savedMQTTPassword;

  response += "\", \"mqtt_topic\":\"";
  response += savedMQTTTopic;

  response += "\", \"mqtt_qos\":\"";
  response += savedMQTTQOS;

  response += "\", \"enable\":\"";
  response += savedEnable;

  response += "\"}";
  server.send(200, "application/json", response);
}

void saveSubData()
{
  if (server.method() == HTTP_POST)
  {
    String savedSSID = server.arg("ssid_name");
    writeData(offsetSSID2, eepromDataLength50, savedSSID);
    delay(1);

    String savedSSIDPassword = server.arg("ssid_password");
    writeData(offsetSSIDPassword2, eepromDataLength50, savedSSIDPassword);
    delay(1);

    String savedMQTTHost = server.arg("mqtt_host");
    writeData(offsetMQTTHost, eepromDataLength50, savedMQTTHost);
    delay(1);

    String savedMQTTPort = server.arg("mqtt_port");
    writeData(offsetMQTTPort, eepromDataLength50, savedMQTTPort);
    delay(1);

    String savedclient = server.arg("mqtt_client");
    writeData(offsetclient, eepromDataLength50, savedclient);
    delay(1);

    String savedMQTTUsername = server.arg("mqtt_username");
    writeData(offsetMQTTUsername, eepromDataLength50, savedMQTTUsername);
    delay(1);

    String savedMQTTPassword = server.arg("mqtt_password");
    writeData(offsetMQTTPassword, eepromDataLength50, savedMQTTPassword);
    delay(1);

    String savedMQTTTopic = server.arg("mqtt_topic");
    writeData(offsetMQTTTopic, eepromDataLength50, savedMQTTTopic);
    delay(1);

    String savedMQTTQOS = server.arg("mqtt_qos");
    writeData(offsetMQTTQOS, eepromDataLength10, savedMQTTQOS);
    delay(1);

    String savedEnable = server.arg("enable");
    writeData(offsetEnable, eepromDataLength10, savedEnable);
    delay(1);
  }
  String message = "{\"status\":\"OK\"}";
  server.send(200, "application/json", message);
}

void writeData(int offset, int length, String value)
{
  int max2 = length;
  int max1 = value.length();
  String result = "";
  for (int l = 0; l < max1; ++l)
  {
    EEPROM.write(offset + l, value[l]);
  }
  for (int l = max1; l < max2; ++l)
  {
    EEPROM.write(offset + l, 0);
  }
  EEPROM.commit();
}

String readData(int offset, int length)
{
  int max = offset + length;
  String result = "";
  for (int l = offset; l < max; ++l)
  {
    char chr = EEPROM.read(l);
    if (chr == 0)
    {
      break;
    }
    result += char(chr);
  }
  result += char('\0');
  return result;
}

String readDataString(int offset, int length)
{
  int max = offset + length;
  String result = "";
  for (int l = offset; l < max; ++l)
  {
    char chr = EEPROM.read(l);
    if (chr == 0)
    {
      break;
    }
    result += char(chr);
  }
  return result;
}
void handleNotFound()
{

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

WiFiClient espClient;
PubSubClient client(espClient);

void mqttCallback(const char *topic, byte *payload, unsigned int length)
{
  String messageTemp = "";
  

  for (int i = 0; i < length; i++)
  {
    messageTemp += (char) payload[i];
  }
  Serial.println(topic);
  Serial.println(messageTemp);

}

String sysEnable = "0";


IPAddress apLocalID(192,168,4,1);
IPAddress apGateway(192,168,4,2);
IPAddress apSubnet(255,255,255,0);

void setup(void)
{
  EEPROM.begin(1024);
  Serial.begin(115200);

  sysEnable = readDataString(offsetEnable, eepromDataLength10);

  if (sysEnable == "1")
  {
    WiFi.setAutoReconnect(true);
    WiFi.setAutoConnect(true);
    WiFi.mode(WIFI_AP_STA);
  }
  else {}

  // Configuration WiFi as Access Point
  String ssidAPS = readDataString(offsetSSID1, eepromDataLength50);
  String passwordAPS = readDataString(offsetSSIDPassword1, eepromDataLength50);
  const char *ssidAP = ssidAPS.c_str();
  const char *passwordAP = passwordAPS.c_str();

  if (ssidAPS.length() > 0)
  {
    WiFi.softAP(ssidAP, passwordAP);
  }
  else
  {
    WiFi.softAP(ssid2, password2);
  }

  WiFi.softAPConfig(apLocalID, apGateway, apSubnet);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  Serial.println("");

  if (sysEnable == "1")
  {
    // Configuration WiFi as Workstation
    String ssidStaS = readDataString(offsetSSID2, eepromDataLength50);
    String passwordStaS = readDataString(offsetSSIDPassword2, eepromDataLength50);
    const char *ssidSta = ssidStaS.c_str();
    const char *passwordSta = passwordStaS.c_str();
    if (ssidStaS.length() > 0)
    {
      WiFi.begin(ssidSta, passwordSta);
    }
    else
    {
      WiFi.begin(ssid, password);
    }

    // Wait for connection
    int trying = 0;
    Serial.println("");
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
      trying++;
      if (trying > 20)
      {
        break;
      }
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    client.setCallback(mqttCallback);

    Serial.println("");
  }

  if (MDNS.begin("esp32"))
  {
    Serial.println("MDNS responder started");
  }

  Serial.println("Before binding path");
  server.on("/", handleRoot);
  server.on("/index.html", handleRoot);
  server.on("/ap-configuration.html", handleAP);
  server.on("/subscribtion-configuration.html", handleSub);
  server.on("/style.css", handleStyle);
  server.on("/script.js", handleScript);
  server.on("/ajax.js", handleScript);
  server.on("/subscribtion-configuration.json", getSubData);
  server.on("/ap-configuration.json", getAPData);
  server.on("/save-subscribtion", saveSubData);
  server.on("/save-ap", saveAPData);
  server.on("/inline", []()
  {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  Serial.println("After binding path");
  server.begin();
  Serial.println("After begin server");

  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(    Task1, "Task1"  // A name just for humans
  , 32768 // This stack size can be checked &adjusted by reading the Stack Highwater
  , NULL, 3 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
  , NULL, 1);

  Serial.println("After task 1");

  xTaskCreatePinnedToCore(    Task2, "Task2", 1024  // Stack size
  , NULL, 1 // Priority
  , NULL, 1);

  Serial.println("After task 2");

  Serial.println("Device is ready");
}

void loop(void)
{
  // HTTP Client must be handled on main loop
  server.handleClient();
  delay(2);
}

void mqttReconnect()
{

  String savedclient = readDataString(offsetclient, eepromDataLength50);
  String savedMQTTUsername = readDataString(offsetMQTTUsername, eepromDataLength50);
  String savedMQTTPassword = readDataString(offsetMQTTPassword, eepromDataLength50);
  String savedMQTTTopic = readDataString(offsetMQTTTopic, eepromDataLength50);
  String savedMQTTQOS = readDataString(offsetMQTTQOS, eepromDataLength50);

  String savedMQTTHost = readDataString(offsetMQTTHost, eepromDataLength50);
  String savedMQTTPort = readDataString(offsetMQTTPort, eepromDataLength50);

  const char *clientId = savedclient.c_str();
  const char *mqttUsername = savedMQTTUsername.c_str();
  const char *mqttPassword = savedMQTTPassword.c_str();
  const char *mqttTopic = savedMQTTTopic.c_str();

  if (savedMQTTQOS == "")
  {
    savedMQTTQOS = "0";
  }
  int mqttQOS = savedMQTTQOS.toInt();

  client.setServer(savedMQTTHost.c_str(), savedMQTTPort.toInt());

  // Loop until we're reconnected
  while (!client.connected())
  {
    // Attempt to connect
    if (client.connect(clientId, mqttUsername, mqttPassword))
    {
      boolean sub = client.subscribe(mqttTopic, mqttQOS);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void Task1(void *pvParameters)
{
  (void) pvParameters;
  for (;;)
  {
    if (sysEnable == "1")
    {
      if (!client.connected())
      {
        mqttReconnect();
      }
      client.loop();
    }
    vTaskDelay(2);
  }
}

void Task2(void *pvParameters)
{
  (void) pvParameters;
  for (;;)
  {
    Serial.println("Task2 Running");
    vTaskDelay(1200);
  }
}
