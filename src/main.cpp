#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ConfigPortal8266.h>
#include <ESP8266WebServer.h>

char*               ssid_pfix = (char*)"jongminPortal";
String              user_config_html = ""
                "<p><input type='text' name='yourVar placeholder='Your Variable'";   


const char* ssid = "533-2.4G-4";
const char* password = "kpu123456!";

ESP8266WebServer server(80);

const int       RELAY = 15;
/*
 *  ConfigPortal library to extend and implement the WiFi connected IOT device
 *
 *  Yoonseok Hur
 *
 *  Usage Scenario:
 *  0. copy the example template in the README.md
 *  1. Modify the ssid_pfix to help distinquish your Captive Portal SSID
 *          char   ssid_pfix[];
 *  2. Modify user_config_html to guide and get the user config data through the Captive Portal
 *          String user_config_html;
 *  2. declare the user config variable before setup
 *  3. In the setup(), read the cfg["meta"]["your field"] and assign to your config variable
 *
 */

void turnOn(){
    Serial.println("turn on");
    digitalWrite(RELAY, 1);
    server.send(200, "text/plain", "turnon");
}

void turnOff(){
    Serial.println("turn off");
    digitalWrite(RELAY, 0);
    server.send(200, "text/plain", "turnoff");
}

void handleRoot(){
  String message = (server.method() == HTTP_GET)?"GET":"POST";
  message += " " + server.uri() + "\n";
  for(uint8_t i=0; i < server.args(); i++){
    message += " " + server.argName(i) + " : " + server.arg(i) + "\n";
  }
  message += "\nHello from ESP8266\n";
  server.send(200, "text/plain", message);

}

void handleNotFound(){
  String message = "File Not Found\n\n";
  server.send(404, "text/plain", message);
}


void setup() {
    Serial.begin(115200);
    pinMode(RELAY, OUTPUT);

    loadConfig();
    // *** If no "config" is found or "config" is not "done", run configDevice ***
    if(!cfg.containsKey("config") || strcmp((const char*)cfg["config"], "done")) {
        configDevice();
    }
    WiFi.mode(WIFI_STA);
    WiFi.begin((const char*)cfg["ssid"], (const char*)cfg["w_pw"]);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // main setup
    Serial.printf("\nIP address : "); Serial.println(WiFi.localIP());

    if (MDNS.begin("jongminPortal")) {
        Serial.println("MDNS responder started");
    }    
    
    server.on("/", handleRoot);

    server.on("/inline", [](){
      server.send(200, "text/plain", "Hello from the inline function\n");
    });

    server.onNotFound(handleNotFound);

    server.on("/turnOn", turnOn);
    server.on("/turnOff", turnOff);

    // replay to all requests with same HTML
    server.begin();
    Serial.println("HTTP server started");

}

void loop() {
    MDNS.update();
    server.handleClient();
}