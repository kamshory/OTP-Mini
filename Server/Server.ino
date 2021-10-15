#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

char *ssid              = "PLANET BIRU";
char *password          = "burungperkutut";

char *ssid2             = "OTP-Mini";
char *password2         = "OTP-Mini";

int eepromSizeString50  = 50;
int eepromSizeString20  = 20;
int eepromSizeInt       = 10;
int eepromSizeBoolean   = 2;

int offsetSSID1         = 0;    // 50
int offsetSSID2         = 50;   // 50
int offsetAPIP          = 100;  // 20
int offsetAPGateway     = 120;  // 20
int offsetAPSubnet      = 140;  // 20
int offsetAPHidden      = 160;  // 2

int offsetSSIDPassword1 = 200;  // 50
int offsetSSIDPassword2 = 250;  // 50
int offsetMQTTHost      = 300;  // 50
int offsetMQTTPort      = 350;  // 10
int offsetclient        = 360;  // 50
int offsetMQTTUsername  = 410;  // 50
int offsetMQTTPassword  = 460;  // 50
int offsetMQTTTopic     = 510;  // 50
int offsetMQTTQOS       = 560;  // 10
int offsetEnable        = 570;  // 50

String sysEnable = "0";


String savedclient = "";
String savedMQTTUsername = "";
String savedMQTTPassword = "";
String savedMQTTTopic = "";
String savedMQTTQOS = "";
String savedMQTTHost = "";
String savedMQTTPort = "";
String savedEnable = "";
int mqttQOS = 0;

IPAddress apLocalID(192,168,4,1);
IPAddress apGateway(192,168,4,2);
IPAddress apSubnet(255,255,255,0);

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
  String response = "<!DOCTYPE html><html lang=\"en\"><head> <meta charset=\"UTF-8\"> <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>Access Point Configuration</title> <link rel=\"stylesheet\" href=\"style.css\"> <script src=\"ajax.js\"></script></head><body> <div class=\"all\"> <h3>Access Point Configuration</h3> <form action=\"\" method=\"post\"> <div class=\"form-item\"> <div class=\"form-label\">SSID</div><div class=\"form-input\"> <input type=\"text\" name=\"ssid_name\" id=\"ssid_name\"> </div></div><div class=\"form-item\"> <div class=\"form-label\">Password</div><div class=\"form-input\"> <input type=\"password\" name=\"ssid_password\" id=\"ssid_password\"> </div></div><div class=\"form-item\"> <div class=\"form-label\">IP Address</div><div class=\"form-input\"> <input type=\"ipaddress\" name=\"ip\" id=\"ip\" placeholder=\"192.168.4.1\"> </div></div><div class=\"form-item\"> <div class=\"form-label\">Gateway</div><div class=\"form-input\"> <input type=\"ipaddress\" name=\"gateway\" id=\"gateway\" placeholder=\"192.168.4.1\"> </div></div><div class=\"form-item\"> <div class=\"form-label\">Subnet</div><div class=\"form-input\"> <input type=\"ipaddress\" name=\"subnet\" id=\"subnet\" placeholder=\"255.255.255.0\"> </div></div><div class=\"form-item\"> <div class=\"form-label\">Hidden</div><div class=\"form-input\"> <select name=\"hidden\" id=\"hidden\"> <option value=\"0\">No</option> <option value=\"1\">Yes</option> </select> </div></div><div class=\"form-item\"> <div class=\"row\"> <div class=\"column\"><input class=\"btn btn-success\" type=\"button\" name=\"save\" id=\"save\" value=\"Save\" onclick=\"return saveAPData();\"></div><div class=\"column\"><input class=\"btn btn-danger\" type=\"button\" name=\"save\" id=\"home\" value=\"Home\" onclick=\"window.location='index.html';\"></div></div></div></form> </div></body></html>";
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
  String response = "body{margin:0;padding:0;position:relative;font-family:Verdana,Geneva,Tahoma,sans-serif;font-size:13px;color:#555}h3{margin:4px 0}.all{width:900px;max-width:100%;margin:auto;padding:20px;box-sizing:border-box}.form-label{padding:5px 0}.form-input{position:relative}.form-input input[type=\"text\"],.form-input input[type=\"number\"],.form-input input[type=\"password\"],.form-input input[type=\"ipaddress\"],.form-input select{width:100%;box-sizing:border-box;padding:6px 10px;border-radius:3px;border:1px solid #CCC;background-color:#FFF;margin-bottom:2px}.form-input input[type=\"ipaddress\"].invalid-ip{border:1px solid #C00}.form-input input[type=\"text\"]:focus,.form-input input[type=\"number\"]:focus,.form-input input[type=\"password\"]:focus,.form-input input[type=\"ipaddress\"]:focus,.form-input select:focus{outline:none}.btn{width:100%;box-sizing:border-box;padding:6px 10px;border-radius:3px;border:1px solid #bdbcbc;background-color:#c7c6c6;color:#333;margin:8px 0}.btn-success{color:#FFF;border:1px solid #46992d;background-color:#419129}.btn-primary{color:#FFF;border:1px solid #2d5899;background-color:#3464ac}.btn-warning{color:#212529;border:1px solid #ffc107;background-color:#ffc107}.btn-danger{color:#FFF;border:1px solid #dc3545;background-color:#dc3545}.row{display:flex;gap:10px}.column{flex:50%;justify-content:space-between}";
  server.sendHeader("Cache-Control", "public, max-age=2678400");
  server.send(200, "text/css", response);
}

void handleScript()
{
  String response = "var ajax={};function saveSubData(){var e=document.querySelector(\"#ssid_name\").value,t=document.querySelector(\"#ssid_password\").value,n=document.querySelector(\"#mqtt_host\").value,o=document.querySelector(\"#mqtt_port\").value,a=document.querySelector(\"#mqtt_client\").value,r=document.querySelector(\"#mqtt_username\").value,u=document.querySelector(\"#mqtt_password\").value,s=document.querySelector(\"#mqtt_topic\").value,c=document.querySelector(\"#mqtt_qos\").value,i=document.querySelector(\"#enable\").value;return ajax.post(\"save-subscribtion\",{action:\"save-subscribtion\",ssid_name:e,ssid_password:t,mqtt_host:n,mqtt_port:o,mqtt_client:a,mqtt_username:r,mqtt_password:u,mqtt_topic:s,mqtt_qos:c,enable:i},function(e){},!0),!1}function loadSubData(){ajax.get(\"subscribtion-configuration.json\",{},function(e){try{var t=JSON.parse(e);document.querySelector(\"#ssid_name\").value=t.ssid_name,document.querySelector(\"#ssid_password\").value=t.ssid_password,document.querySelector(\"#mqtt_host\").value=t.mqtt_host,document.querySelector(\"#mqtt_port\").value=t.mqtt_port,document.querySelector(\"#mqtt_client\").value=t.mqtt_client,document.querySelector(\"#mqtt_username\").value=t.mqtt_username,document.querySelector(\"#mqtt_password\").value=t.mqtt_password,document.querySelector(\"#mqtt_topic\").value=t.mqtt_topic,document.querySelector(\"#mqtt_qos\").value=t.mqtt_qos,document.querySelector(\"#enable\").value=t.enable}catch(e){}},!0)}function loadAPData(){ajax.get(\"ap-configuration.json\",{},function(e){try{var t=JSON.parse(e);document.querySelector(\"#ssid_name\").value=t.ssid_name,document.querySelector(\"#ssid_password\").value=t.ssid_password,document.querySelector(\"#ip\").value=t.ip,document.querySelector(\"#gateway\").value=t.gateway,document.querySelector(\"#subnet\").value=t.subnet,document.querySelector(\"#hidden\").value=t.hidden}catch(e){}},!0)}function saveAPData(){var e=document.querySelector(\"#ssid_name\").value,t=document.querySelector(\"#ssid_password\").value,n=document.querySelector(\"#ip\").value,o=document.querySelector(\"#gateway\").value,a=document.querySelector(\"#subnet\").value,r=document.querySelector(\"#hidden\").value;return ajax.post(\"save-ap\",{action:\"save-ap\",ssid_name:e,ssid_password:t,ip:n,gateway:o,subnet:a,hidden:r},function(e){},!0),!1}function handleIP(e){e=e.target;isValidIP(e.value)?e.classList.remove(\"invalid-ip\"):(e.classList.remove(\"invalid-ip\"),e.classList.add(\"invalid-ip\"))}function isValidIP(e){if(0==e.length)return!0;var t,n=e.split(\".\");if(4!=n.length)return!1;for(t in n){if(isNaN(parseInt(n[t])))return!1;if(n[t]<0||255<n[t])return!1}return!0}ajax.create=function(){if(\"undefined\"!=typeof XMLHttpRequest)return new XMLHttpRequest;for(var e,t=[\"MSXML2.XmlHttp.6.0\",\"MSXML2.XmlHttp.5.0\",\"MSXML2.XmlHttp.4.0\",\"MSXML2.XmlHttp.3.0\",\"MSXML2.XmlHttp.2.0\",\"Microsoft.XmlHttp\"],n=0;n<t.length;n++)try{e=new ActiveXObject(t[n]);break}catch(e){}return e},ajax.send=function(e,t,n,o,a){void 0===a&&(a=!0);var r=ajax.create();r.open(n,e,a),r.onreadystatechange=function(){4==r.readyState&&t(r.responseText)},\"POST\"==n&&r.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\"),r.send(o)},ajax.get=function(e,t,n,o){var a,r=[];for(a in t)t.hasOwnProperty(a)&&r.push(encodeURIComponent(a)+\"=\"+encodeURIComponent(t[a]));ajax.send(e+(r.length?\"?\"+r.join(\"&\"):\"\"),n,\"GET\",null,o)},ajax.post=function(e,t,n,o){var a,r=[];for(a in t)t.hasOwnProperty(a)&&r.push(encodeURIComponent(a)+\"=\"+encodeURIComponent(t[a]));ajax.send(e,n,\"POST\",r.join(\"&\"),o)},window.onload=function(){var e=window.location.toString();-1<e.indexOf(\"ap-configuration.html\")&&loadAPData(),-1<e.indexOf(\"subscribtion-configuration.html\")&&loadSubData();const t=document.querySelectorAll('input[type=\"ipaddress\"]');if(t.length)for(var n=0;n<t.length;n++)t[n].addEventListener(\"keyup\",function(e){handleIP(e)}),t[n].addEventListener(\"change\",function(e){handleIP(e)})};";
  server.sendHeader("Cache-Control", "public, max-age=2678400");
  server.send(200, "text/javascript", response);
}

void getAPData()
{
  String savedSSID = readDataString(offsetSSID1, eepromSizeString50);
  String savedSSIDPassword = readDataString(offsetSSIDPassword1, eepromSizeString50);

  String savedIP = readDataString(offsetAPIP, eepromSizeString20);
  String savedGateway = readDataString(offsetAPGateway, eepromSizeString20);
  String savedSubnet = readDataString(offsetAPSubnet, eepromSizeString20);
  String savedHidden = readDataString(offsetAPHidden, eepromSizeBoolean);
  String response = "";
  response += "{\"ssid_name\":\"";
  response += savedSSID;
  response += "\", \"ssid_password\":\"";
  response += savedSSIDPassword;
  response += "\", \"ip\":\"";
  response += savedIP;
  response += "\", \"gateway\":\"";
  response += savedGateway;
  response += "\", \"subnet\":\"";
  response += savedSubnet;
  response += "\", \"hidden\":\"";
  response += savedHidden;
  response += "\"}";
  server.send(200, "application/json", response);
}

void saveAPData()
{
  if (server.method() == HTTP_POST)
  {
    String savedSSID = server.arg("ssid_name");
    writeData(offsetSSID1, eepromSizeString50, savedSSID);
    String savedSSIDPassword = server.arg("ssid_password");
    writeData(offsetSSIDPassword1, eepromSizeString50, savedSSIDPassword);
    String savedIP = server.arg("ip");
    writeData(offsetAPIP, eepromSizeString20, savedIP);
    String savedGateway = server.arg("gateway");
    writeData(offsetAPGateway, eepromSizeString20, savedGateway);
    String savedSubnet = server.arg("subnet");
    writeData(offsetAPSubnet, eepromSizeString20, savedSubnet);
    String savedHidden = server.arg("hidden");
    writeData(offsetAPHidden, eepromSizeBoolean, savedHidden);  
  }
  String message = "{\"status\":\"OK\"}";
  server.send(200, "application/json", message);
}

void resetAP()
{
  String savedSSID = "OTP-Mini";
  writeData(offsetSSID1, eepromSizeString50, savedSSID);
  String savedSSIDPassword = "OTP-Mini";
  writeData(offsetSSIDPassword1, eepromSizeString50, savedSSIDPassword);
  
  String savedIP = "";
  writeData(offsetAPIP, eepromSizeString20, savedIP);
  String savedGateway = "";
  writeData(offsetAPGateway, eepromSizeString20, savedGateway);
  String savedSubnet = "";
  writeData(offsetAPSubnet, eepromSizeString20, savedSubnet);
  String savedHidden = "0";
  writeData(offsetAPHidden, eepromSizeBoolean, savedHidden);
}

void resetSTA()
{
  String savedSSID = "PLANET BIRU";
  String savedSSIDPassword = "kodokterbang";
  writeData(offsetSSID2, eepromSizeString50, savedSSID);
  writeData(offsetSSIDPassword2, eepromSizeString50, savedSSIDPassword);
}

void getSubData()
{

  String response = "";
  String savedSSID = readDataString(offsetSSID2, eepromSizeString50);
  String savedSSIDPassword = readDataString(offsetSSIDPassword2, eepromSizeString50);
  String savedMQTTHost = readDataString(offsetMQTTHost, eepromSizeString50);
  String savedMQTTPort = readDataString(offsetMQTTPort, eepromSizeInt);
  String savedclient = readDataString(offsetclient, eepromSizeString50);
  String savedMQTTUsername = readDataString(offsetMQTTUsername, eepromSizeString50);
  String savedMQTTPassword = readDataString(offsetMQTTPassword, eepromSizeString50);
  String savedMQTTTopic = readDataString(offsetMQTTTopic, eepromSizeString50);
  String savedMQTTQOS = readDataString(offsetMQTTQOS, eepromSizeInt);
  String savedEnable = readDataString(offsetEnable, eepromSizeBoolean);

  

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
    writeData(offsetSSID2, eepromSizeString50, savedSSID);
    delay(1);

    String savedSSIDPassword = server.arg("ssid_password");
    writeData(offsetSSIDPassword2, eepromSizeString50, savedSSIDPassword);
    delay(1);

     savedMQTTHost = server.arg("mqtt_host");
    writeData(offsetMQTTHost, eepromSizeString50, savedMQTTHost);
    delay(1);

    String savedMQTTPort = server.arg("mqtt_port");
    writeData(offsetMQTTPort, eepromSizeInt, savedMQTTPort);
    delay(1);

    String savedclient = server.arg("mqtt_client");
    writeData(offsetclient, eepromSizeString50, savedclient);
    delay(1);

     savedMQTTUsername = server.arg("mqtt_username");
    writeData(offsetMQTTUsername, eepromSizeString50, savedMQTTUsername);
    delay(1);

     savedMQTTPassword = server.arg("mqtt_password");
    writeData(offsetMQTTPassword, eepromSizeString50, savedMQTTPassword);
    delay(1);

     savedMQTTTopic = server.arg("mqtt_topic");
    writeData(offsetMQTTTopic, eepromSizeString50, savedMQTTTopic);
    delay(1);

     savedMQTTQOS = server.arg("mqtt_qos");
    writeData(offsetMQTTQOS, eepromSizeInt, savedMQTTQOS);
    delay(1);

     savedEnable = server.arg("enable");
    writeData(offsetEnable, eepromSizeBoolean, savedEnable);
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
WiFiClient espClient2;
PubSubClient client(espClient);
PubSubClient client2(espClient2);

void mqttCallback(const char *topic, byte *payload, unsigned int length)
{
  String message = "";
 
  for (int i = 0; i < length; i++)
  {
    message += (char) payload[i];
  }
  
  DynamicJsonDocument json(1024);
  deserializeJson(json, message);
  const char * command = json["command"];
  const char * responseTopic = json["callback_topic"];
  int callbackDelay = json["callback_delay"];
  delay(callbackDelay);
  publishMessage(responseTopic, message.c_str());

}


void setup(void)
{
  EEPROM.begin(1024);
  Serial.begin(115200);

  sysEnable = readDataString(offsetEnable, eepromSizeBoolean);

  if (sysEnable == "1")
  {
    WiFi.setAutoReconnect(true);
    WiFi.setAutoConnect(true);
    WiFi.mode(WIFI_AP_STA);
  }
  else {}

  // Configuration WiFi as Access Point
  String ssidAPS = readDataString(offsetSSID1, eepromSizeString50);
  String passwordAPS = readDataString(offsetSSIDPassword1, eepromSizeString50);
 
  if (ssidAPS.length() > 0)
  {
    String sip = readDataString(offsetAPIP, eepromSizeString20);
    String sgw = readDataString(offsetAPGateway, eepromSizeString20);
    String ssn = readDataString(offsetAPSubnet, eepromSizeString20);
    String shd = readDataString(offsetAPHidden, eepromSizeBoolean);

    IPAddress test1(10,10,10,10);
 
    if(sip.length()>6 && sgw.length()>6 && ssn.length()>6 
    && test1.fromString(sip) && test1.fromString(sgw) && test1.fromString(ssn))
    {
      apLocalID.fromString(sip);
      apGateway.fromString(sgw);
      apSubnet.fromString(ssn);
    }

    WiFi.softAP(ssidAPS.c_str(), passwordAPS.c_str(), 1, (int) shd.toInt(), 10);
    WiFi.softAPConfig(apLocalID, apGateway, apSubnet);
  }
  else
  {
    WiFi.softAP(ssid2, password2, 1, 0, 10);
    WiFi.softAPConfig(apLocalID, apGateway, apSubnet);
  }

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  

  Serial.println("");

  if (sysEnable == "1")
  {
    // Configuration WiFi as Workstation
    String ssidStaS = readDataString(offsetSSID2, eepromSizeString50);
    String passwordStaS = readDataString(offsetSSIDPassword2, eepromSizeString50);
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
  server.begin();

  getMQTTConfig();
  
  // Now set up two tasks to run independently.
  xTaskCreate(    Task1, "Task1"  // A name just for humans
  , 32768 // This stack size can be checked &adjusted by reading the Stack Highwater
  , NULL, 1 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
  , NULL);

  
  xTaskCreate(    
    Task2
  , "Task2"
  , 1024  // Stack size
  , NULL, 1 // Priority
  , NULL);

  
  Serial.println("Device is ready");

  
  
  //resetAP();
}

void loop(void)
{
  // HTTP Client must be handled on main loop
  server.handleClient();
  delay(2);
}



void getMQTTConfig()
{
  savedclient = readDataString(offsetclient, eepromSizeString50);
  savedMQTTUsername = readDataString(offsetMQTTUsername, eepromSizeString50);
  savedMQTTPassword = readDataString(offsetMQTTPassword, eepromSizeString50);
  savedMQTTTopic = readDataString(offsetMQTTTopic, eepromSizeString50);
  savedMQTTQOS = readDataString(offsetMQTTQOS, eepromSizeString50);
  savedMQTTHost = readDataString(offsetMQTTHost, eepromSizeString50);
  savedMQTTPort = readDataString(offsetMQTTPort, eepromSizeInt);

  if (savedMQTTQOS == "")
  {
    savedMQTTQOS = "0";
  }
  mqttQOS = savedMQTTQOS.toInt();
}

void publishMessage(const char * topic, const char * message)
{
  client2.setServer(savedMQTTHost.c_str(), savedMQTTPort.toInt());

  // Loop until we're reconnected
  while (!client.connected())
  {
    // Attempt to connect
    Serial.println("Connecting...");
    Serial.println(savedMQTTHost.c_str());
    Serial.println(savedMQTTPort.toInt());
    Serial.println(savedclient.c_str());
    Serial.println(savedMQTTUsername.c_str());
    Serial.println(savedMQTTPassword.c_str());
    if (client.connect(savedclient.c_str(), savedMQTTUsername.c_str(), savedMQTTPassword.c_str()))
    {
      break;
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client2.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
  delay(1000);
  client.publish(topic, message);
  Serial.println("Published...");
  Serial.println(topic);
  Serial.println(message);
}

void mqttReconnect()
{

  

 
  

  client.setServer(savedMQTTHost.c_str(), savedMQTTPort.toInt());

  // Loop until we're reconnected
  
  while (!client.connected())
  {
    // Attempt to connect
    Serial.println("Connecting...");
    if (client.connect(savedclient.c_str(), savedMQTTUsername.c_str(), savedMQTTPassword.c_str()))
    {
      boolean sub = client.subscribe(savedMQTTTopic.c_str(), mqttQOS);
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
    vTaskDelay(10);
  }
}

void Task2(void *pvParameters)
{
  (void) pvParameters;
  for (;;)
  {
    vTaskDelay(1200);
  }
}
