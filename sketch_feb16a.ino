#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#define ESSID "ESP001"
#define EPASS "12345678"
String API_KEY = "CO49DGYWAXVM16OA";
const char* serverAPI = "api.thingspeak.com";
char cssid[20], cpass[20];

volatile byte indx;

ESP8266WebServer server(80);
WiFiClient esp;


void get_send_data(){
    String data ; 
    while(1){
      if(Serial.available()>0)// check buffer 
      {
        byte c = Serial.read();
        if(c<48 || c>57) break;
       else{
        data += char(c);
        }
      }
      else break;
      }
   if(esp.connect(serverAPI,80)&& data.length()){
    String buff  = "GET /update?api_key="+API_KEY+"&field1=";
    buff += data;
    buff += '\n';
    if(esp.connected()){
      esp.print(buff);
      delay(2000);
      }
    }  
    
}



void loadConfig(){
  int i;
  for(i = 0; i < 20; i++){
    if(EEPROM.read(i) != 0){
      cssid[i] = EEPROM.read(i);
    }
    else break;
  }
  for(i = 20; i < 40; i++){
    if(EEPROM.read(i) != 0){
      cpass[i - 20] = EEPROM.read(i);
    }
    else break;
  }
}
const char configPage[] PROGMEM = R"EOF(
<html>
<head>
<title>Setup Wifi</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
h1{
  color: red;
  margin-left: 40px;
  margin-bottom: 50px
}
.inp{
  width: 300px;
  height: 30px;
  margin-bottom: 15px;
}
.btn{
  margin-left: 80px;
  padding: 10px 40px; 
  background: #50CFFD; 
  color: #fff;
  cursor: pointer;
  border: none
}
.btn:hover{
  opacity: 0.7
}
</style>
</head>
<body>
<form method="POST" action="/setup" style="width: 400px; margin: auto; margin-top: 200px">
<h1>WIFI CONFIG</h1>
<input name="ssid" placeholder="WIFI SSID" class="inp">
<input type="password" name="pass" placeholder="WIFI PASSWORD" class="inp">
<button type="submit" class="btn">Connect</button>
</form>
</body>
</html>
)EOF"; 
void get_config_page(){
  server.send(200,"text/html",configPage);
}



void setup() {
  Serial.begin(9600);
  loadConfig();
  
  int count = 0;
//  Serial.println(cssid);
//  Serial.println(cpass);
//  WiFi.begin(cssid,cpass);
//  Serial.print("Connecting to ");
//  Serial.println(cssid);
//  Serial.println(cpass);
  while(WiFi.status() != WL_CONNECTED && count< 50){
//      Serial.print(".");
//      Serial.print(count);
      delay(100);
      count ++;
    }
  if(WiFi.status() != WL_CONNECTED){
  WiFi.softAP(ESSID,EPASS);
//  Serial.print("Soft-AP IP address = ");
//  Serial.println(WiFi.softAPIP());
  server.on("/",get_config_page);
  server.on("/setup",setup_wifi);
  server.begin();
  }
// else{Serial.print("Connected!");}
}

void loop() {
  // put your main code here, to run repeatedly:
  
  server.handleClient(); 
  if(WiFi.status() == WL_CONNECTED){
      get_send_data();
      
    }
}


void setup_wifi(){
  int count = 0; 
  
//  Serial.println(server.arg("ssid"));
//  Serial.println(server.arg("pass"));
  WiFi.begin(server.arg("ssid"),server.arg("pass"));
 // WiFi.begin("Noru_GPIO","29042001");
//  Serial.print("Connecting to ");
//  Serial.print(server.arg("ssid"));
  while(WiFi.status() != WL_CONNECTED && count< 50){
//      Serial.print(".");
//      Serial.print(count);
      delay(100);
      count ++;
    }
  if(WiFi.status() == WL_CONNECTED){
    
      int i;
    for(int i = 0; i < 40; i++){
      EEPROM.write(i, 0);
    }
    EEPROM.commit();
    for(i = 0; i < server.arg("ssid").length(); i++){
      EEPROM.write(i, server.arg("ssid")[i]);
    }
    for(i = 0; i < server.arg("pass").length(); i++){
      EEPROM.write(i + 20, server.arg("pass")[i]);
    }
    EEPROM.commit();
    delay(100);
//    loadConfig();
//    Serial.print(cssid);
//    Serial.println(cpass);
    server.send(200,"text/html","<html><script> alert(\"Setup thanh cong!\"); </script></html>");
    
}
    
  else server.send(200,"text/html","<html><script> alert(\"Setup that bai, vui long thu lai!\"); </script></html>");
  
}
