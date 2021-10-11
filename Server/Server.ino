#include <PubSubClient.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>


char *ssid              = "PLANET BIRU";
char *password          = "kodokterbang";

char *ssid2             = "OTP-Mini";
char *password2         = "OTP-Mini";

int eepromDataLength    = 50;
int offsetSSID1         = 0;
int offsetSSID2         = 50;
int offsetSSIDPassword1 = 100;
int offsetSSIDPassword2 = 150;
int offsetMQTTHost      = 200;
int offsetMQTTPort      = 250;
int offsetclient        = 300;
int offsetMQTTUsername  = 350;
int offsetMQTTPassword  = 400;

WebServer server(80);

// Define two tasks for Task1 & Task2
void Task1( void *pvParameters );
void Task2( void *pvParameters );

void handleRoot() {
    String response = "<!DOCTYPE html><html lang=\"en\"><head> <meta charset=\"UTF-8\"> <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>OTP-Mini</title> <link rel=\"stylesheet\" href=\"style.css\"> <script src=\"ajax.js\"></script></head><body> <div class=\"all\"> <h3>OTP-Mini</h3> <div class=\"form-item\"> <div class=\"row\"> <div class=\"column\"><input class=\"btn btn-success\" type=\"button\" name=\"save\" id=\"sub\" value=\"Subscribtion\" onclick=\"window.location='subscribtion-configuration.html';\"></div><div class=\"column\"><input class=\"btn btn-success\" type=\"button\" name=\"save\" id=\"ap\" value=\"Access Point\" onclick=\"window.location='ap-configuration.html';\"></div></div></div></div></body></html>";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/html", response);
}

void handleAP() {
    String response = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Access Point Configuration</title><link rel=\"stylesheet\" href=\"style.css\"><script src=\"ajax.js\"></script></head><body><div class=\"all\"><h3>Access Point Configuration</h3><form action=\"\" method=\"post\"><div class=\"form-item\"><div class=\"form-label\">SSID</div><div class=\"form-input\"><input type=\"text\" name=\"ssid_name\" id=\"ssid_name\"></div></div><div class=\"form-item\"><div class=\"form-label\">Password</div><div class=\"form-input\"><input type=\"password\" name=\"ssid_password\" id=\"ssid_password\"></div></div><div class=\"form-item\"><div class=\"row\"><div class=\"column\"><input class=\"btn btn-success\" type=\"button\" name=\"save\" id=\"save\" value=\"Save\" onclick=\"return saveAPData();\"></div><div class=\"column\"><input class=\"btn btn-danger\" type=\"button\" name=\"save\" id=\"home\" value=\"Home\" onclick=\"window.location='index.html';\"></div></div></div></form></div></body></html>";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/html", response);
}

void handleSub() {
    String response = "<!DOCTYPE html><html lang=\"en\"><head> <meta charset=\"UTF-8\"> <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>Subscribtion Configuration</title> <link rel=\"stylesheet\" href=\"style.css\"> <script src=\"ajax.js\"></script></head><body> <div class=\"all\"> <h3>Subscribtion Configuration</h3> <form action=\"\" method=\"post\"> <div class=\"form-item\"> <div class=\"form-label\">SSID</div><div class=\"form-input\"> <input type=\"text\" name=\"ssid_name\" id=\"ssid_name\"> </div></div><div class=\"form-item\"> <div class=\"form-label\">Password</div><div class=\"form-input\"> <input type=\"password\" name=\"ssid_password\" id=\"ssid_password\"> </div></div><div class=\"form-item\"> <div class=\"form-label\">MQTT Host</div><div class=\"form-input\"> <input type=\"text\" name=\"mqtt_host\" id=\"mqtt_host\"> </div></div><div class=\"form-item\"> <div class=\"form-label\">MQTT Port</div><div class=\"form-input\"> <input type=\"text\" name=\"mqtt_port\" id=\"mqtt_port\"> </div></div><div class=\"form-item\"> <div class=\"form-label\">MQTT Client ID</div><div class=\"form-input\"> <input type=\"text\" name=\"mqtt_client\" id=\"mqtt_client\"> </div></div><div class=\"form-item\"> <div class=\"form-label\">MQTT Username</div><div class=\"form-input\"> <input type=\"text\" name=\"mqtt_username\" id=\"mqtt_username\"> </div></div><div class=\"form-item\"> <div class=\"form-label\">MQTT Password</div><div class=\"form-input\"> <input type=\"password\" name=\"mqtt_password\" id=\"mqtt_password\"> </div></div><div class=\"form-item\"> <div class=\"row\"> <div class=\"column\"><input class=\"btn btn-success\" type=\"button\" name=\"save\" id=\"save\" value=\"Save\" onclick=\"return saveSubData();\"></div><div class=\"column\"><input class=\"btn btn-danger\" type=\"button\" name=\"save\" id=\"home\" value=\"Home\" onclick=\"window.location='index.html';\"></div></div></div></form> </div></body></html>";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/html", response);
}

void handleStyle()
{
    String response = "body{margin:0;padding:0;position:relative;font-family:Verdana,Geneva,Tahoma,sans-serif;font-size:13px;color:#555}h3{margin:4px 0}.all{width:400px;max-width:100%;margin:auto;padding:20px;box-sizing:border-box}.form-label{padding:5px 0}.form-input{position:relative}.form-input input[type=\"text\"],.form-input input[type=\"number\"],.form-input input[type=\"password\"]{width:100%;box-sizing:border-box;padding:6px 10px;border-radius:3px;border:1px solid #CCC;background-color:#FFF;margin-bottom:2px}.form-input input[type=\"text\"]:focus,.form-input input[type=\"password\"]:focus{outline:none}.btn{width:100%;box-sizing:border-box;padding:6px 10px;border-radius:3px;border:1px solid #bdbcbc;background-color:#c7c6c6;color:#333;margin:8px 0}.btn-success{color:#FFF;border:1px solid #46992d;background-color:#419129}.btn-primary{color:#FFF;border:1px solid #2d5899;background-color:#3464ac}.btn-warning{color:#212529;border:1px solid #ffc107;background-color:#ffc107}.btn-danger{color:#FFF;border:1px solid #dc3545;background-color:#dc3545}.row{display:flex;gap:10px}.column{flex:50%;justify-content:space-between}";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/css", response);
}

void handleScript()
{
    String response = "var ajax={};function saveSubData(){var e=document.querySelector(\"#ssid_name\").value,t=document.querySelector(\"#ssid_password\").value,o=document.querySelector(\"#mqtt_host\").value,n=document.querySelector(\"#mqtt_port\").value,a=document.querySelector(\"#mqtt_client\").value,s=document.querySelector(\"#mqtt_username\").value,r=document.querySelector(\"#mqtt_password\").value;return ajax.post(\"save-subscribtion\",{action:\"save-subscribtion\",ssid_name:e,ssid_password:t,mqtt_host:o,mqtt_port:n,mqtt_client:a,mqtt_username:s,mqtt_password:r},function(e){},!0),!1}function loadSubData(){ajax.get(\"subscribtion-configuration.json\",{},function(e){e=JSON.parse(e);document.querySelector(\"#ssid_name\").value=e.ssid_name,document.querySelector(\"#ssid_password\").value=e.ssid_password,document.querySelector(\"#mqtt_host\").value=e.mqtt_host,document.querySelector(\"#mqtt_port\").value=e.mqtt_port,document.querySelector(\"#mqtt_client\").value=e.mqtt_client,document.querySelector(\"#mqtt_username\").value=e.mqtt_username,document.querySelector(\"#mqtt_password\").value=e.mqtt_password},!0)}function loadAPData(){ajax.get(\"ap-configuration.json\",{},function(e){e=JSON.parse(e);document.querySelector(\"#ssid_name\").value=e.ssid_name,document.querySelector(\"#ssid_password\").value=e.ssid_password},!0)}function saveAPData(){var e=document.querySelector(\"#ssid_name\").value,t=document.querySelector(\"#ssid_password\").value;return ajax.post(\"save-ap\",{action:\"save-ap\",ssid_name:e,ssid_password:t},function(e){},!0),!1}ajax.create=function(){if(\"undefined\"!=typeof XMLHttpRequest)return new XMLHttpRequest;for(var e,t=[\"MSXML2.XmlHttp.6.0\",\"MSXML2.XmlHttp.5.0\",\"MSXML2.XmlHttp.4.0\",\"MSXML2.XmlHttp.3.0\",\"MSXML2.XmlHttp.2.0\",\"Microsoft.XmlHttp\"],o=0;o<t.length;o++)try{e=new ActiveXObject(t[o]);break}catch(e){}return e},ajax.send=function(e,t,o,n,a){void 0===a&&(a=!0);var s=ajax.create();s.open(o,e,a),s.onreadystatechange=function(){4==s.readyState&&t(s.responseText)},\"POST\"==o&&s.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\"),s.send(n)},ajax.get=function(e,t,o,n){var a,s=[];for(a in t)t.hasOwnProperty(a)&&s.push(encodeURIComponent(a)+\"=\"+encodeURIComponent(t[a]));ajax.send(e+(s.length?\"?\"+s.join(\"&\"):\"\"),o,\"GET\",null,n)},ajax.post=function(e,t,o,n){var a,s=[];for(a in t)t.hasOwnProperty(a)&&s.push(encodeURIComponent(a)+\"=\"+encodeURIComponent(t[a]));ajax.send(e,o,\"POST\",s.join(\"&\"),n)},window.onload=function(){var e=window.location.toString();-1<e.indexOf(\"ap-configuration.html\")&&loadAPData(),-1<e.indexOf(\"subscribtion-configuration.html\")&&loadSubData()};";
    server.sendHeader("Cache-Control", "public, max-age=2678400");
    server.send(200, "text/javascript", response);
}

void getAPData()
{
    String savedSSID = readDataString(offsetSSID1, eepromDataLength);
    String savedSSIDPassword = readDataString(offsetSSIDPassword1, eepromDataLength);
    String response = "";
    response += "{\"ssid_name\":\"";
    response += savedSSID;
    response += "\", \"ssid_password\":\"";
    response += savedSSIDPassword;
    response += "\"}";
    server.send(200, "application/json", response);
}

void saveAPData() {
    if(server.method() == HTTP_POST)
    {
        String savedSSID = server.arg("ssid_name");
        writeData(offsetSSID1, eepromDataLength, savedSSID);
        String savedSSIDPassword = server.arg("ssid_password");
        writeData(offsetSSIDPassword1, eepromDataLength, savedSSIDPassword);
    }
    String message = "{\"status\":\"OK\"}";
    server.send(200, "application/json", message); 
}

void resetAP()
{
    String savedSSID = "OTP-Mini";
    writeData(offsetSSID1, eepromDataLength, savedSSID);
    String savedSSIDPassword = "OTP-Mini";
    writeData(offsetSSIDPassword1, eepromDataLength, savedSSIDPassword);
}

void getSubData()
{
    String savedSSID = readDataString(offsetSSID2, eepromDataLength);
    String savedSSIDPassword = readDataString(offsetSSIDPassword2, eepromDataLength);
    String savedMQTTHost = readDataString(offsetMQTTHost, eepromDataLength);
    String savedMQTTPort = readDataString(offsetMQTTPort, eepromDataLength);
    String savedclient = readDataString(offsetclient, eepromDataLength);
    String savedMQTTUsername = readDataString(offsetMQTTUsername, eepromDataLength);
    String savedMQTTPassword = readDataString(offsetMQTTPassword, eepromDataLength);

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

    response += "\"}";
    server.send(200, "application/json", response);
}

void saveSubData() {
    if(server.method() == HTTP_POST)
    {
        String savedSSID = server.arg("ssid_name");
        writeData(offsetSSID2, eepromDataLength, savedSSID);
        delay(2);

        String savedSSIDPassword = server.arg("ssid_password");
        writeData(offsetSSIDPassword2, eepromDataLength, savedSSIDPassword);
        delay(2);

        String savedMQTTHost = server.arg("mqtt_host");
        writeData(offsetMQTTHost, eepromDataLength, savedMQTTHost);
        delay(2);

        String savedMQTTPort = server.arg("mqtt_port");
        writeData(offsetMQTTPort, eepromDataLength, savedMQTTPort);
        delay(2);

        String savedclient = server.arg("mqtt_client");
        writeData(offsetclient, eepromDataLength, savedclient);
        delay(2);

        String savedMQTTUsername = server.arg("mqtt_username");
        writeData(offsetMQTTUsername, eepromDataLength, savedMQTTUsername);
        delay(2);

        String savedMQTTPassword = server.arg("mqtt_password");
        writeData(offsetMQTTPassword, eepromDataLength, savedMQTTPassword);
        delay(2);
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
        result += char(EEPROM.read(l));
    }
    return result;
}

String readDataString(int offset, int length)
{
    int max = offset + length;
    String result = "";
    for (int l = offset; l < max; ++l)
    {
        char chr = EEPROM.read(l);
        if(chr == 0)
        {
            break;
        }
        result += char(chr);
    }
    return result;
}
void handleNotFound() {

    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message); 
}

EthernetClient ethClient;
PubSubClient client(ethClient);

void setup(void) {

    EEPROM.begin(1024);
    Serial.begin(115200);
    WiFi.mode(WIFI_AP_STA);

    WiFi.setAutoReconnect(true);
    WiFi.setAutoConnect(true);

    // Configuration WiFi as Workstation
    String ssidStaS = readDataString(offsetSSID2, eepromDataLength);
    String passwordStaS = readDataString(offsetSSIDPassword2, eepromDataLength);
    const char *ssidSta = ssidStaS.c_str();
    const char *passwordSta = passwordStaS.c_str();
    if(ssidStaS.length() > 0)
    {
        WiFi.begin(ssidSta, passwordSta);
    }
    else
    {
        WiFi.begin(ssid, password);
    }


    // Configuration WiFi as Access Point
    String ssidAPS = readDataString(offsetSSID1, eepromDataLength);
    String passwordAPS = readDataString(offsetSSIDPassword1, eepromDataLength);
    const char *ssidAP = ssidAPS.c_str();
    const char *passwordAP = passwordAPS.c_str();

    if(ssidAPS.length() > 0)
    {
        WiFi.softAP(ssidAP, passwordAP);
    }
    else
    {
        WiFi.softAP(ssid2, password2);
    }

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    Serial.println("");

    // Wait for connection
    int trying = 0;
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
        trying++;
        if(trying > 20)
        {
            break;
        }
    }

    

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }

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
    server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
    });
    server.onNotFound(handleNotFound);
    server.begin();

    // Now set up two tasks to run independently.
    xTaskCreatePinnedToCore(
    Task1
    ,  "Task1"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL 
    ,  1);

    xTaskCreatePinnedToCore(
    Task2
    ,  "Task2"
    ,  1024  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL 
    ,  1);

    
    Serial.println("Device is ready");
}

void loop(void) {
    // HTTP Client must be handled on main loop
    server.handleClient();
    delay(2);
}

void mqttCallback(char* topic, byte* message, unsigned int length) {
  char * mqttTopic = "sms";
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print("Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == mqttTopic) {
    
  }
}

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192, 168, 4, 1);

void mqttReconnect() {
  char * mqttTopic = "sms";
  char * clientId = "kamshory";
  char * mqttUsername = "kamshory";
  char * mqttPassword = "kamshory";
  Ethernet.begin(mac, ip);
  client.setServer("192.168.1.3", 1883);
  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to ");
    Serial.print("192.168.1.3:1883 ");
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(mqttTopic);
    } else {
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
        Serial.println("Task1 Running"); 
        delay(5000);
        
        if (!client.connected()) {
          mqttReconnect();
        }
        client.loop();
        
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
