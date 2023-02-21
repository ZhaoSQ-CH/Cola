#include "wifi_config.h"
#include "image_debug.h"

#include <WiFi.h>

// Replace with your network credentials
const char* ssid_AP = "ESP32-DY";
const char* password_AP = "xxxxxxxx";

const char* ssid_STA = "deyanoffice1";
const char* password_STA = "xxxxxxxx";
void wifi_init(void) {
#if WIFI_CONFIG_STA
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid_STA, password_STA);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
#if WIFI_DEBUG_ENABLE
        Serial.print(".");
#endif  
    }

#if WIFI_DEBUG_ENABLE
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("Camera Stream Ready! Go to: http://");
    Serial.println(WiFi.localIP());
#endif
#endif

#if WIFI_CONFIG_AP
    WiFi.softAP(ssid_AP,password_AP);  //打开ESP32热点
#if WIFI_DEBUG_ENABLE
    Serial.println("");
    Serial.print("SSID: ");
    Serial.println(ssid_AP);
    Serial.print("PASSWORD: ");
    Serial.println(password_AP);
    Serial.print("Camera Stream Ready! Go to: http://");
    Serial.println(WiFi.softAPIP());  //串口输出模块IP地址
#endif
#endif
}
