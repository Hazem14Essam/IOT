/**********************************************************
 Title:Sendind data to NodeMCU by Local Machine to Machine (M2M)
 Author:Hazem essam
 Board:NODEMCU
 **********************************************************/
 //Libraries
 /**************to make NODEMCU as Access point****************************/ 
#include <ESP8266WiFi.h>
 /**************to create web server on NODEMCU********************************/
#include <WiFiClient.h>
#include <ESP8266WebServer.h> 
//incuding library of lcd
#include<Wire.h>
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2);//creating object and giving it address of lcd and number of rows and numbers of colums
//incuding library of DHT sensor
#include<DHT.h>
int temp_c=0;
int hum=0;
const byte DHT_PIN=D6;
const byte DHT_TYPE=11;//MODULE
DHT dht(DHT_PIN,DHT_TYPE);
const byte LED=D7;
//GAS sensor
int digital=120;
int digital_width;
//WiFI network info
const char ssid[]="Etisalat-x66p";
const char password[]="nader01065592884";
//Network :static IP details
IPAddress serverIP(192,168,1,10);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
//create web Server
ESP8266WebServer server(80);
//web page
char http_updates_content[1400];
char htp_page_content[1400]="\
<!DOCTYPE html>\
<html>\
<head>\
<title>M2M</title>\
<meta http-equiv=\"refresh\"content=\"5; url=http://192.168.1.10\"/>\
<meta charset=\"utf-8\">\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css\">\
<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js\"></script>\
<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/js/bootstrap.min.js\"></script>\
</head>\
<body>\
<div class=\"container\">\
<h3>temperature</h3>\
<div class=\"progress\">\
<div class=\"progress-bar progress-bar-success progress-bar-striped active\" role=\"progressbar\" style=\"width:%d%%\">\
%d C\
</div>\
</div>\
<h3>Humidity</h3>\
<div class=\"progress\">\
<div class=\"progress-bar progress-bar-success progress-bar-striped active\" role=\"progressbar\" style=\"width:%d%%\">\
%d\
</div>\
</div>\
<h3>Gas</h3>\
<div class=\"progress\">\
<div class=\"progress-bar progress-bar-success progress-bar-striped active\" role=\"progressbar\" style=\"width:%d%%\">\
%d\
</div>\
</div>\
<a href=\"\?relay=on\" class=\"btn btn-danger btn-lg\" role=\"button\">Relay ON</a>\
<a href=\"\?relay=off\" class=\"btn btn-danger btn-lg\" role=\"button\">Relay OFF</a>\
</div>\
</body>\
</html>\
";
void setup() {
/*connect to my wifi */
WiFi.config(serverIP,gateway,subnet);//giving web server static ip
WiFi.begin(ssid,password);
//LED
pinMode(LED,OUTPUT);
digitalWrite(LED,LOW);
//DHT sensor
dht.begin();
//intialize lcd
lcd.init();
lcd.backlight();
lcd.print("connecting");
lcd.clear();
lcd.setCursor(0,1);
lcd.print(".");
//wait for connection
byte count =0;
while (WiFi.status()!=WL_CONNECTED)
{
  delay(500);
  count++;
  lcd.setCursor(count,1);
  lcd.print(".");
}
//handle server pages
server.on("/",handleRoot);
server.onNotFound(handleNotFound);
server.begin();
lcd.clear();
lcd.print("server started");
lcd.setCursor(0,1);
lcd.print(WiFi.localIP());

 }

void loop() {
  server.handleClient();
}
void handleNotFound()
{
  server.send(404,"text/html","<h1>page not found</h1>");
}
void handleRoot()
{
 if(server.argName(0)=="relay")//chech on the variable(led) from request
 {
  if(server.arg(0)=="on")//check on the value of variable
  {
    digitalWrite(LED,HIGH);
  }
  else if(server.arg(0)=="off")
  {
    digitalWrite(LED,LOW);
  }
 }

hum=dht.readHumidity();
//hum=70;
temp_c=dht.readTemperature();
//temp_c=25;
if(isnan(hum)){hum=0;}
if(isnan(temp_c)){temp_c=0;}
//gas sensor
digital=analogRead(A0);
//digital=120;
digital_width=(digital*100)/500;
snprintf(http_updates_content,1300,htp_page_content,temp_c,temp_c,hum,hum,digital_width,digital);
server.send(200,"text/html",http_updates_content);
}
