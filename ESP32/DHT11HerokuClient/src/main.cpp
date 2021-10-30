#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#include "time.h"

#define SSID                "Hsun"
#define PASSWD              "3.14159265358979"

// #define HEROHU_URL          "http://192.168.1.192/data/setDHT11Data"
#define HEROHU_URL         "http://aiot1025.herokuapp.com/data/setDHT11Data"

#define DHT_PIN             13
#define DHT_TYPE            DHT11

#define UTC                 8
#define NTP_SERVER         "pool.ntp.org"
#define GMTOFFSET_SEC     ((UTC) * 60 * 60)
#define DAYLIGHTOFFSET_SEC  0

#define DELAY_TIME          100

#define SAVE_LEN            6
#define SEND_DATA_LEN     ((SAVE_LEN) - 1)

#define LED_PIN             2

DHT dht(DHT_PIN, DHT_TYPE);

unsigned long saveTime = 0UL;
String payload;

void setup() {
    Serial.begin(115200);
    Serial.printf("\n");

    pinMode(LED_PIN, OUTPUT);

    WiFi.begin(SSID, PASSWD);

    while(WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());

    configTime(GMTOFFSET_SEC, DAYLIGHTOFFSET_SEC, NTP_SERVER);
    
    payload.reserve(100);
    dht.begin();
    delay(2000);
}

void loop() {
    if(millis() > saveTime) {
        saveTime += 60000UL;
        struct tm timeinfo;
        while(!getLocalTime(&timeinfo));
        int temp, hum;
        
        do {
            temp = int(dht.readTemperature());
            hum  = int(dht.readHumidity());
            delay(2000);
        } while (temp > 100 || hum > 100);

        Serial.printf("Temp: %d\n", temp);
        Serial.printf("Hum: %d\n", hum);
        
        payload  = "{\"Name\":\"setDHT11Data\",\"Year\":";
        payload += (timeinfo.tm_year + 1900); 
        payload += ",\"Month\":";
        payload += (timeinfo.tm_mon + 1); 
        payload += ",\"Day\":";
        payload += timeinfo.tm_mday;
        payload += ",\"Hour\":";
        payload += timeinfo.tm_hour;
        payload += ",\"Min\":";
        payload += timeinfo.tm_min;
        payload += ",\"Temp\":";
        payload +=  temp;
        payload += ",\"Hum\":";
        payload +=  hum;
        payload += "}";

        Serial.println(payload);

        HTTPClient httpClient;

        httpClient.begin(HEROHU_URL);
        httpClient.addHeader("Content-Type", "application/json");
        
        int httpResponseCode = httpClient.POST(payload);

        payload.clear();
        payload.end();

        if (httpResponseCode > 0) {
            digitalWrite(LED_PIN, HIGH);
            Serial.printf("HTTP Response code: %d\n", httpResponseCode);
            Serial.println(httpClient.getString());
        }
        else {
            digitalWrite(LED_PIN, LOW);
            Serial.printf("Error code: %d\n", httpResponseCode);
            ESP.restart();
        }

        httpClient.end();
    }
}
