/****************************************************************************************************************************
  powerprox.ino
  ESP8266 WiFi Bridge for Power Prox main CPU
  Provides a bridge to WiFi for the main Freetronics EtherTen CPU, as well as providing some higher-level functions that could
  not fit on the AVR processor

  Dependancies:
  ESPAsync_WiFiManager_Lite (https://github.com/khoih-prog/ESPAsync_WiFiManager_Lite)
  ElegantOTA (https://github.com/ayushsharma82/ElegantOTA) [Using Async Mode]
  *****************************************************************************************************************************/

// Include our requirements here
#include "eaw_config.h"
#include <ESPAsync_WiFiManager_Lite.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <ESP8266mDNS.h>


bool LOAD_DEFAULT_CONFIG_DATA = false;
ESP_WM_LITE_Configuration defaultConfig;
ESPAsync_WiFiManager_Lite* ESPAsync_WiFiManager;
AsyncWebServer webservices(80);
unsigned long ota_progress_millis = 0;


void heartBeatPrint()
{
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
    Serial.print("H");        // H means connected to WiFi
  else
  {
    if (ESPAsync_WiFiManager->isConfigMode())
      Serial.print("C");        // C means in Config Mode
    else
      Serial.print("F");        // F means not connected to WiFi
  }

  if (num == 80)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(F(" "));
  }
}

void check_status()
{
  static unsigned long checkstatus_timeout = 0;

  // Print hearbeat every HEARTBEAT_INTERVAL (20) seconds.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    heartBeatPrint();
    checkstatus_timeout = millis() + HEARTBEAT_INTERVAL;
  }
}


void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\n\nStarting ESPAsync_WiFi using "));
  Serial.print(FS_Name);
  Serial.print(F(" on "));
  Serial.println(ARDUINO_BOARD);
  Serial.println(ESP_ASYNC_WIFI_MANAGER_LITE_VERSION);
  Serial.println(ESP_MULTI_RESET_DETECTOR_VERSION);

  ESPAsync_WiFiManager = new ESPAsync_WiFiManager_Lite();
  
  // Set customized AP SSID and PWD
  ESPAsync_WiFiManager->setConfigPortal(AP_SSID, AP_PWD);
  ESPAsync_WiFiManager->setConfigPortalChannel(0);
  ESPAsync_WiFiManager->setCustomsStyle(PSTR("<style>div,input{padding:5px;font-size:1em;}input{width:95%;}body{text-align: center;}button{background-color:blue;color:white;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.3rem;margin:0px;}</style>"));
  ESPAsync_WiFiManager->setCustomsHeadElement(PSTR("<style>html{filter: invert(10%);}</style>"));
  ESPAsync_WiFiManager->setCORSHeader(PSTR("Your Access-Control-Allow-Origin"));
  ESPAsync_WiFiManager->begin(HOST_NAME);

  // Create a callback for the main page here....
  webservices.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! This is a sample response.  Go to /update to run over-the-air updates.");
  });
  // Route for root / web page
  // webservices.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  //   if(!request->authenticate(http_username, http_password))
  //     return request->requestAuthentication();
  //   request->send_P(200, "text/html", index_html, processor);
  // });
    
  // webservices.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(401);
  // });

  // webservices.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send_P(200, "text/html", logout_html, processor);
  // });

  // Send a GET request to <ESP_IP>/update?state=<inputMessage>
  // webservices.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
  //   if(!request->authenticate(http_username, http_password))
  //     return request->requestAuthentication();
  //   String inputMessage;
  //   String inputParam;
  //   // GET input1 value on <ESP_IP>/update?state=<inputMessage>
  //   if (request->hasParam(PARAM_INPUT_1)) {
  //     inputMessage = request->getParam(PARAM_INPUT_1)->value();
  //     inputParam = PARAM_INPUT_1;
  //     digitalWrite(output, inputMessage.toInt());
  //   }
  //   else {
  //     inputMessage = "No message sent";
  //     inputParam = "none";
  //   }
  //   Serial.println(inputMessage);
  //   request->send(200, "text/plain", "OK");
  // });

  // Set up the OTA callback functions
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);
}


void loop()
{
  static enum events {NONE, CONNECTED, DISCONNECTED} nw_events = NONE;
  if ((WiFi.status() == WL_CONNECTED) && (nw_events != CONNECTED)) {
    // Start AsyncElegantOTA
    #ifdef ESP_WM_LITE_DEBUG_OUTPUT
    Serial.println("Starting main webserver");
    #endif
    ElegantOTA.begin(&webservices);
    webservices.begin();
    MDNS.begin(ESPAsync_WiFiManager->getBoardName());
    MDNS.addService("http", "tcp", 80);
    nw_events = CONNECTED;
  }
  else if (WiFi.status() != WL_CONNECTED) {
    if (nw_events == CONNECTED) {
      // Do we need to do this?  Assume so, as otherwise it would clash with the config portal?
      #ifdef ESP_WM_LITE_DEBUG_OUTPUT
      Serial.println("Killing main webserver");
      #endif
      webservices.end();
      MDNS.end();
    }
    nw_events = DISCONNECTED;
  }
  if (nw_events == CONNECTED) {
    ElegantOTA.loop();
    MDNS.update();
  }
  ESPAsync_WiFiManager->run();
  check_status();

}
