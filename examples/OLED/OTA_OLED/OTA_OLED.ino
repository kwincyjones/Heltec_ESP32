/*
 * HelTec Automation(TM) WIFI_LoRa_32 factory test code which includes
 *  the follow functions:
 *
 * - Basic OLED function test;
 *
 * - Basic serial port test(in baud rate 115200);
 *
 * - Basic LED test;
 *
 * - WIFI join and scan test;
 *
 * - ArduinoOTA By Wifi;
 *
 * - Timer test and some other Arduino basic functions.
 *
 * by lxyzn from HelTec AutoMation, ChengDu, China
 * 成都惠利特自动化科技有限公司¸
 * www.heltec.cn
 *
 * This project is also available on GitHub:
 * https://github.com/HelTecAutomation/Heltec_ESP32
*/


#include <ArduinoOTA.h>
#include <WiFi.h>
#include <Wire.h>
#include "heltec.h"


/**********************************************  WIFI Client æ³¨æ„ç¼–è¯‘æ—¶è¦è®¾ç½®æ­¤å€?*********************************
 * wifi client
 */
const char* ssid = "xxxxxx"; //replace "xxxxxx" with your WIFI's ssid
const char* password = "xxxxxx"; //replace "xxxxxx" with your WIFI's password

//WiFi&OTA å‚æ•°
//#define HOSTNAME "HelTec_OTA_OLED"
#define PASSWORD "HT.123456" //the password for OTA upgrade, can set it in any char you want


#define USE_STATIC_IP false
#if USE_STATIC_IP
  IPAddress staticIP(192,168,1,22);
  IPAddress gateway(192,168,1,9);
  IPAddress subnet(255,255,255,0);
  IPAddress dns1(8, 8, 8, 8);
  IPAddress dns2(114,114,114,114);
#endif

/*******************************************************************
 * OLED Arguments
 */
//#define RST_OLED 16                     //OLED Resetå¼•è„šï¼Œéœ€è¦æ‰‹åŠ¨Resetï¼Œå¦åˆ™ä¸æ˜¾ç¤º
#define OLED_UPDATE_INTERVAL 500        //OLEDå±å¹•åˆ·æ–°é—´éš”ms
//SSD1306 display(0x3C, 4, 15);           //å¼•è„š4ï¼?5æ˜¯ç»‘å®šåœ¨Kit 32çš„ä¸»æ¿ä¸Šçš„ï¼Œä¸èƒ½åšå…¶å®ƒç”¨


/********************************************************************
 * OTAå‡çº§é…ç½®
 */
void setupOTA()
{
  //Port defaults to 8266
  //ArduinoOTA.setPort(8266);

  //Hostname defaults to esp8266-[ChipID]
//  ArduinoOTA.setHostname(HOSTNAME);

  //No authentication by default
  ArduinoOTA.setPassword(PASSWORD);

  //Password can be set with it's md5 value as well
  //MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  //ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]()
  {
    Heltec.display->clear();
    Heltec.display->setFont(ArialMT_Plain_10);        //è®¾ç½®å­—ä½“å¤§å°
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);//è®¾ç½®å­—ä½“å¯¹é½æ–¹å¼
    Heltec.display->drawString(0, 0, "Start Updating....");

    Serial.printf("Start Updating....Type:%s\n", (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem");
  });

  ArduinoOTA.onEnd([]()
  {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Update Complete!");
    Serial.println("Update Complete!");

    ESP.restart();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    String pro = String(progress / (total / 100)) + "%";
    int progressbar = (progress / (total / 100));
    //int progressbar = (progress / 5) % 100;
    //int pro = progress / (total / 100);

    Heltec.display->clear();
#if defined (WIRELESS_STICK)
    Heltec.display->drawProgressBar(0, 11, 64, 8, progressbar);    // draw the progress bar
    Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);          // draw the percentage as String
    Heltec.display->drawString(10, 20, pro);
#else
    Heltec.display->drawProgressBar(0, 32, 120, 10, progressbar);    // draw the progress bar
    Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);          // draw the percentage as String
    Heltec.display->drawString(64, 15, pro);
#endif
    Heltec.display->display();

    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error)
  {
    Serial.printf("Error[%u]: ", error);
    String info = "Error Info:";
    switch(error)
    {
      case OTA_AUTH_ERROR:
        info += "Auth Failed";
        Serial.println("Auth Failed");
        break;

      case OTA_BEGIN_ERROR:
        info += "Begin Failed";
        Serial.println("Begin Failed");
        break;

      case OTA_CONNECT_ERROR:
        info += "Connect Failed";
        Serial.println("Connect Failed");
        break;

      case OTA_RECEIVE_ERROR:
        info += "Receive Failed";
        Serial.println("Receive Failed");
        break;

      case OTA_END_ERROR:
        info += "End Failed";
        Serial.println("End Failed");
        break;
    }

    Heltec.display->clear();
    Heltec.display->drawString(0, 0, info);
    ESP.restart();
  });

  ArduinoOTA.begin();
}

/*********************************************************************
 * setup wifi
 */
void setupWIFI()
{
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Connecting...");
  Heltec.display->drawString(0, 10, String(ssid));
  Heltec.display->display();

  //è¿žæŽ¥WiFiï¼Œåˆ é™¤æ—§çš„é…ç½®ï¼Œå…³é—­WIFIï¼Œå‡†å¤‡é‡æ–°é…ç½?
  WiFi.disconnect(true);
  delay(1000);

  WiFi.mode(WIFI_STA);
  //WiFi.onEvent(WiFiEvent);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);    //æ–­å¼€WiFiåŽè‡ªåŠ¨é‡æ–°è¿žæŽ?ESP32ä¸å¯ç”?
  //WiFi.setHostname(HOSTNAME);
  WiFi.begin(ssid, password);
#if USE_STATIC_IP
  WiFi.config(staticIP, gateway, subnet);
#endif

  //ç­‰å¾…5000msï¼Œå¦‚æžœæ²¡æœ‰è¿žæŽ¥ä¸Šï¼Œå°±ç»§ç»­å¾€ä¸?
  //ä¸ç„¶åŸºæœ¬åŠŸèƒ½ä¸å¯ç”?
  byte count = 0;
  while(WiFi.status() != WL_CONNECTED && count < 10)
  {
    count ++;
    delay(500);
    Serial.print(".");
  }

  Heltec.display->clear();
  if(WiFi.status() == WL_CONNECTED)
    Heltec.display->drawString(0, 0, "Connected");
  else
    Heltec.display->drawString(0, 0, "Connect False");
  Heltec.display->display();
}

/******************************************************
 * arduino setup
 */
void setup()
{
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  pinMode(25, OUTPUT);
  digitalWrite(25,HIGH);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Initialize...");

  setupWIFI();
  setupOTA();
}

/******************************************************
 * arduino loop
 */
void loop()
{
  ArduinoOTA.handle();
  unsigned long ms = millis();
  if(ms % 1000 == 0)
  {
    Serial.println("helloï¼ŒOTA now");
  }
}

/****************************************************
 * [é€šç”¨å‡½æ•°]ESP32 WiFi Kit 32äº‹ä»¶å¤„ç†
 */
void WiFiEvent(WiFiEvent_t event)
{
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch(event)
    {
        case SYSTEM_EVENT_WIFI_READY:               /**< ESP32 WiFi ready */
            break;
        case SYSTEM_EVENT_SCAN_DONE:                /**< ESP32 finish scanning AP */
            break;

        case SYSTEM_EVENT_STA_START:                /**< ESP32 station start */
            break;
        case SYSTEM_EVENT_STA_STOP:                 /**< ESP32 station stop */
            break;

        case SYSTEM_EVENT_STA_CONNECTED:            /**< ESP32 station connected to AP */
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:         /**< ESP32 station disconnected from AP */
            break;

        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:      /**< the auth mode of AP connected by ESP32 station changed */
            break;

        case SYSTEM_EVENT_STA_GOT_IP:               /**< ESP32 station got IP from connected AP */
        case SYSTEM_EVENT_STA_LOST_IP:              /**< ESP32 station lost IP and the IP is reset to 0 */
            break;

        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:       /**< ESP32 station wps succeeds in enrollee mode */
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:        /**< ESP32 station wps fails in enrollee mode */
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:       /**< ESP32 station wps timeout in enrollee mode */
        case SYSTEM_EVENT_STA_WPS_ER_PIN:           /**< ESP32 station wps pin code in enrollee mode */
            break;

        case SYSTEM_EVENT_AP_START:                 /**< ESP32 soft-AP start */
        case SYSTEM_EVENT_AP_STOP:                  /**< ESP32 soft-AP stop */
        case SYSTEM_EVENT_AP_STACONNECTED:          /**< a station connected to ESP32 soft-AP */
        case SYSTEM_EVENT_AP_STADISCONNECTED:       /**< a station disconnected from ESP32 soft-AP */
        case SYSTEM_EVENT_AP_PROBEREQRECVED:        /**< Receive probe request packet in soft-AP interface */
        case SYSTEM_EVENT_AP_STA_GOT_IP6:           /**< ESP32 station or ap interface v6IP addr is preferred */
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            break;

        case SYSTEM_EVENT_ETH_START:                /**< ESP32 ethernet start */
        case SYSTEM_EVENT_ETH_STOP:                 /**< ESP32 ethernet stop */
        case SYSTEM_EVENT_ETH_CONNECTED:            /**< ESP32 ethernet phy link up */
        case SYSTEM_EVENT_ETH_DISCONNECTED:         /**< ESP32 ethernet phy link down */
        case SYSTEM_EVENT_ETH_GOT_IP:               /**< ESP32 ethernet got IP from connected AP */
        case SYSTEM_EVENT_MAX:
            break;
    }
}
