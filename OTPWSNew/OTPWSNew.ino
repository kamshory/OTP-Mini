#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

int onboardLED             = 2;

char *ssid                 = "PLANET BIRU";
char *password             = "burungperkutut";
char *ssid2                = "OTP-Mini";
char *password2            = "OTP-Mini";

int eepromSizeString50     = 50;
int eepromSizeString20     = 20;
int eepromSizeInt          = 10;
int eepromSizeBoolean      = 2;

int offsetSSID1            = 0;    // 50
int offsetSSID2            = 50;   // 50
int offsetAPIP             = 100;  // 20
int offsetAPGateway        = 120;  // 20
int offsetAPSubnet         = 140;  // 20
int offsetAPHidden         = 160;  // 2
int offsetSSIDPassword1    = 200;  // 50
int offsetSSIDPassword2    = 250;  // 50
int offsetWSHost           = 300;  // 50
int offsetWSPort           = 350;  // 10
int offsetWSPath           = 360;  // 50
int offsetWSUsername       = 410;  // 50
int offsetWSPassword       = 460;  // 50
int offsetWSTopic          = 510;  // 50
int offsetEnable           = 570;  // 2

String savedWSPath         = "";
String savedWSUsername     = "";
String savedWSPassword     = "";
String savedWSTopic        = "";
String savedWSHost         = "";
String savedWSPort         = "";
String savedEnable         = "";

String sysEnable           = "0"; 
boolean connected          = false;
boolean lastState          = false;
long lastDisconnected      = millis();
long reconnectWiFiTreshold = 10000;

const char * gTopic        = "";
const char * gMessage      = "";

IPAddress apLocalID(192,168,4,1);
IPAddress apGateway(192,168,4,2);
IPAddress apSubnet(255,255,255,0);
WebServer server(80);
WebSocketsClient webSocket;

void Task1(void *pvParameters);
void Task2(void *pvParameters);

String urlDecode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str.length(); i++){
        c=str.charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str.charAt(i);
        i++;
        code1=str.charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
        
        encodedString+=c;  
      }
      
      yield();
    }
    
   return encodedString;
}

String urlEncode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
    
}

unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

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
  String response = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Subscribtion Configuration</title><link rel=\"stylesheet\" href=\"style.css\"><script src=\"ajax.js\"></script></head><body><div class=\"all\"><h3>Subscribtion Configuration</h3><form action=\"\" method=\"post\"><div class=\"form-item\"><div class=\"form-label\">SSID</div><div class=\"form-input\"><input type=\"text\" name=\"ssid_name\" id=\"ssid_name\"></div></div><div class=\"form-item\"><div class=\"form-label\">Password</div><div class=\"form-input\"><input type=\"password\" name=\"ssid_password\" id=\"ssid_password\"></div></div><div class=\"form-item\"><div class=\"form-label\">WS Host</div><div class=\"form-input\"><input type=\"text\" name=\"ws_host\" id=\"ws_host\"></div></div><div class=\"form-item\"><div class=\"form-label\">WS Port</div><div class=\"form-input\"><input type=\"text\" name=\"ws_port\" id=\"ws_port\"></div></div><div class=\"form-item\"><div class=\"form-label\">WS Path</div><div class=\"form-input\"><input type=\"text\" name=\"ws_client\" id=\"ws_client\"></div></div><div class=\"form-item\"><div class=\"form-label\">WS Username</div><div class=\"form-input\"><input type=\"text\" name=\"ws_username\" id=\"ws_username\"></div></div><div class=\"form-item\"><div class=\"form-label\">WS Password</div><div class=\"form-input\"><input type=\"password\" name=\"ws_password\" id=\"ws_password\"></div></div><div class=\"form-item\"><div class=\"form-label\">Topic</div><div class=\"form-input\"><input type=\"text\" name=\"ws_topic\" id=\"ws_topic\"></div></div><div class=\"form-item\"><div class=\"form-label\">Enable</div><div class=\"form-input\"><select name=\"enable\" id=\"enable\"><option value=\"0\">No</option><option value=\"1\">Yes</option></select></div></div><div class=\"form-item\"><div class=\"row\"><div class=\"column\"><input class=\"btn btn-success\" type=\"button\" name=\"save\" id=\"save\" value=\"Save\" onclick=\"return saveSubData();\"></div><div class=\"column\"><input class=\"btn btn-danger\" type=\"button\" name=\"save\" id=\"home\" value=\"Home\" onclick=\"window.location='index.html';\"></div></div></div></form></div></body></html>";
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
  String response = "var ajax={};function saveSubData(){var e=document.querySelector(\"#ssid_name\").value,t=document.querySelector(\"#ssid_password\").value,n=document.querySelector(\"#ws_host\").value,o=document.querySelector(\"#ws_port\").value,a=document.querySelector(\"#ws_client\").value,r=document.querySelector(\"#ws_username\").value,u=document.querySelector(\"#ws_password\").value,s=document.querySelector(\"#ws_topic\").value,i=document.querySelector(\"#enable\").value;return ajax.post(\"save-subscribtion\",{action:\"save-subscribtion\",ssid_name:e,ssid_password:t,ws_host:n,ws_port:o,ws_client:a,ws_username:r,ws_password:u,ws_topic:s,enable:i},function(e){},!0),!1}function loadSubData(){ajax.get(\"subscribtion-configuration.json\",{},function(e){try{var t=JSON.parse(e);document.querySelector(\"#ssid_name\").value=t.ssid_name,document.querySelector(\"#ssid_password\").value=t.ssid_password,document.querySelector(\"#ws_host\").value=t.ws_host,document.querySelector(\"#ws_port\").value=t.ws_port,document.querySelector(\"#ws_client\").value=t.ws_client,document.querySelector(\"#ws_username\").value=t.ws_username,document.querySelector(\"#ws_password\").value=t.ws_password,document.querySelector(\"#ws_topic\").value=t.ws_topic,document.querySelector(\"#enable\").value=t.enable}catch(e){}},!0)}function loadAPData(){ajax.get(\"ap-configuration.json\",{},function(e){try{var t=JSON.parse(e);document.querySelector(\"#ssid_name\").value=t.ssid_name,document.querySelector(\"#ssid_password\").value=t.ssid_password,document.querySelector(\"#ip\").value=t.ip,document.querySelector(\"#gateway\").value=t.gateway,document.querySelector(\"#subnet\").value=t.subnet,document.querySelector(\"#hidden\").value=t.hidden}catch(e){}},!0)}function saveAPData(){var e=document.querySelector(\"#ssid_name\").value,t=document.querySelector(\"#ssid_password\").value,n=document.querySelector(\"#ip\").value,o=document.querySelector(\"#gateway\").value,a=document.querySelector(\"#subnet\").value,r=document.querySelector(\"#hidden\").value;return ajax.post(\"save-ap\",{action:\"save-ap\",ssid_name:e,ssid_password:t,ip:n,gateway:o,subnet:a,hidden:r},function(e){},!0),!1}function handleIP(e){e=e.target;isValidIP(e.value)?e.classList.remove(\"invalid-ip\"):(e.classList.remove(\"invalid-ip\"),e.classList.add(\"invalid-ip\"))}function isValidIP(e){if(0==e.length)return!0;var t,n=e.split(\".\");if(4!=n.length)return!1;for(t in n){if(isNaN(parseInt(n[t])))return!1;if(n[t]<0||255<n[t])return!1}return!0}ajax.create=function(){if(\"undefined\"!=typeof XMLHttpRequest)return new XMLHttpRequest;for(var e,t=[\"MSXML2.XmlHttp.6.0\",\"MSXML2.XmlHttp.5.0\",\"MSXML2.XmlHttp.4.0\",\"MSXML2.XmlHttp.3.0\",\"MSXML2.XmlHttp.2.0\",\"Microsoft.XmlHttp\"],n=0;n<t.length;n++)try{e=new ActiveXObject(t[n]);break}catch(e){}return e},ajax.send=function(e,t,n,o,a){void 0===a&&(a=!0);var r=ajax.create();r.open(n,e,a),r.onreadystatechange=function(){4==r.readyState&&t(r.responseText)},\"POST\"==n&&r.setRequestHeader(\"Content-type\",\"application/x-www-form-urlEncoded\"),r.send(o)},ajax.get=function(e,t,n,o){var a,r=[];for(a in t)t.hasOwnProperty(a)&&r.push(encodeURIComponent(a)+\"=\"+encodeURIComponent(t[a]));ajax.send(e+(r.length?\"?\"+r.join(\"&\"):\"\"),n,\"GET\",null,o)},ajax.post=function(e,t,n,o){var a,r=[];for(a in t)t.hasOwnProperty(a)&&r.push(encodeURIComponent(a)+\"=\"+encodeURIComponent(t[a]));ajax.send(e,n,\"POST\",r.join(\"&\"),o)},window.onload=function(){var e=window.location.toString();-1<e.indexOf(\"ap-configuration.html\")&&loadAPData(),-1<e.indexOf(\"subscribtion-configuration.html\")&&loadSubData();const t=document.querySelectorAll('input[type=\"ipaddress\"]');if(t.length)for(var n=0;n<t.length;n++)t[n].addEventListener(\"keyup\",function(e){handleIP(e)}),t[n].addEventListener(\"change\",function(e){handleIP(e)})};";
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
  String savedWSHost = readDataString(offsetWSHost, eepromSizeString50);
  String savedWSPort = readDataString(offsetWSPort, eepromSizeInt);
  String savedWSPath = readDataString(offsetWSPath, eepromSizeString50);
  String savedWSUsername = readDataString(offsetWSUsername, eepromSizeString50);
  String savedWSPassword = readDataString(offsetWSPassword, eepromSizeString50);
  String savedWSTopic = readDataString(offsetWSTopic, eepromSizeString50);
  String savedEnable = readDataString(offsetEnable, eepromSizeBoolean);

  response += "{\"ssid_name\":\"";
  response += savedSSID;

  response += "\", \"ssid_password\":\"";
  response += savedSSIDPassword;

  response += "\", \"ws_host\":\"";
  response += savedWSHost;

  response += "\", \"ws_port\":\"";
  response += savedWSPort;

  response += "\", \"ws_client\":\"";
  response += savedWSPath;

  response += "\", \"ws_username\":\"";
  response += savedWSUsername;

  response += "\", \"ws_password\":\"";
  response += savedWSPassword;

  response += "\", \"ws_topic\":\"";
  response += savedWSTopic;

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

    savedWSHost = server.arg("ws_host");
    writeData(offsetWSHost, eepromSizeString50, savedWSHost);
    delay(1);

    String savedWSPort = server.arg("ws_port");
    writeData(offsetWSPort, eepromSizeInt, savedWSPort);
    delay(1);

    String savedWSPath = server.arg("ws_client");
    writeData(offsetWSPath, eepromSizeString50, savedWSPath);
    delay(1);

    savedWSUsername = server.arg("ws_username");
    writeData(offsetWSUsername, eepromSizeString50, savedWSUsername);
    delay(1);

    savedWSPassword = server.arg("ws_password");
    writeData(offsetWSPassword, eepromSizeString50, savedWSPassword);
    delay(1);

    savedWSTopic = server.arg("ws_topic");
    writeData(offsetWSTopic, eepromSizeString50, savedWSTopic);
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

    Serial.println("");
  }

  if (MDNS.begin("esp32"))
  {
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
  server.on("/inline", []()
  {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();

  getWSConfig();
  
  // Now set up two tasks to run independently.
  xTaskCreate(    
    Task1
  , "Task1"  // A name just for humans
  , 32768    // This stack size can be checked &adjusted by reading the Stack Highwater
  , NULL
  , 1        // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
  , NULL);

  xTaskCreate(    
    Task2
  , "Task2"  // A name just for humans
  , 10240    // This stack size can be checked &adjusted by reading the Stack Highwater
  , NULL
  , 1        // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
  , NULL);
  
  pinMode(onboardLED, OUTPUT);
  
  
  //resetAP();
  //resetSTA();

  wsReconnect();
  Serial.println("Device is ready");
}

void loop(void)
{
  // HTTP Client must be handled by main loop
  server.handleClient();
  delay(2);
}

void getWSConfig()
{
  savedWSPath = readDataString(offsetWSPath, eepromSizeString50);
  savedWSUsername = readDataString(offsetWSUsername, eepromSizeString50);
  savedWSPassword = readDataString(offsetWSPassword, eepromSizeString50);
  savedWSTopic = readDataString(offsetWSTopic, eepromSizeString50);
  savedWSHost = readDataString(offsetWSHost, eepromSizeString50);
  savedWSPort = readDataString(offsetWSPort, eepromSizeInt);
}

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
  String request = "";
  String response = "";
  int i = 0;
  for (i = 0; i < length; i++)
  {
    request += (char) payload[i];
  }
  DynamicJsonDocument json(1024);
  deserializeJson(json, request);
  const char *command = json["command"];
  const char *responseTopic = json["callback_topic"];
  boolean requireResponse = !(responseTopic == NULL);
  
  // Define your program here...
  if(requireResponse)
  {
    response = request;

    int callbackDelay = json["callback_delay"];
    sendResponse(responseTopic, response, callbackDelay);
  }  
}

void sendResponse(const char * responseTopic, String response, int callbackDelay)
{
  String path = savedWSPath;
  path += "?topic=";
  path += urlEncode(String(responseTopic));

  WebSocketsClient webSocket2;
  webSocket2.begin(savedWSHost.c_str(), savedWSPort.toInt(), path.c_str());
  webSocket2.setAuthorization(savedWSUsername.c_str(), savedWSPassword.c_str());

  int i;
  long lastUpdate = millis();

  while(lastUpdate + callbackDelay >= millis() || WiFi.status() != WL_CONNECTED)
  {
    webSocket2.loop();
    if(webSocket2.sendPing())
    {
      break;
    }
    delay(7);
  }
  i = 0;
  while(!webSocket2.sendTXT(response) && i < 10)
  {
    webSocket2.loop();
    delay(10);
    i++;
  }
}
void wsReconnect()
{
  String path = savedWSPath;
  path += "?topic=";
  path += urlEncode(String(savedWSTopic));
  webSocket.begin(savedWSHost.c_str(), savedWSPort.toInt(), path.c_str());
  webSocket.setAuthorization(savedWSUsername.c_str(), savedWSPassword.c_str());
  webSocket.onEvent(webSocketEvent);
}


void Task1(void *pvParameters)
{
  (void) pvParameters;
  for (;;)
  {
    if (sysEnable == "1")
    {
      webSocket.loop();
      if(lastState != connected)
      {
        if(connected)
        {
          digitalWrite(onboardLED, HIGH);
        }
        else
        {
          digitalWrite(onboardLED, LOW);
          lastDisconnected = millis();
        }
        lastState = connected;
      }
    }
    vTaskDelay(2);
  }
}

void Task2(void *pvParameters)
{
  (void) pvParameters;
  for (;;)
  {
    if(!connected)
    {
      if(millis() - lastDisconnected > reconnectWiFiTreshold && WiFi.status() != WL_CONNECTED)
      {
        Serial.println("Reconnecting to WiFi...");
        WiFi.disconnect();
        boolean res = WiFi.reconnect();
        if(res)
        {
          lastDisconnected = millis();
        }
      }
    }
    vTaskDelay(2000);
  }
}
