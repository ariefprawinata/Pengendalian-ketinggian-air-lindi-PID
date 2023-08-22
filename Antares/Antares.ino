#include <ESP8266WiFi.h>
#include <AntaresESP8266MQTT.h>
#include <ArduinoJson.h>
//#include <SoftwareSerial.h>

//Antares
#define ACCESSKEY "c64f7bb751ffc94c:2e9c2ed10774761b"
#define WIFISSID "1"
#define PASSWORD "12345678"

#define projectName "PengendalianAirLindiPID"
#define deviceName "PengendalianAirLindi"

AntaresESP8266MQTT antares(ACCESSKEY);
float busvoltage = 0, current_mA = 0, power_mW = 0;

int callback1 = 0, callback4 = 0;
unsigned long waktuSebelum1 = 0, waktuSebelum2 = 0, waktuSebelum3 = 0;

void getData() {
  DynamicJsonDocument data_(500);
  deserializeJson(data_, Serial);
  callback1 = data_["callback-1"];
  busvoltage = data_["busvoltage"];
  current_mA = data_["current"];
  power_mW = data_["power"];
}

void sendCallback(long millisMain, long interval) {
  DynamicJsonDocument data2(500);
  if((unsigned long) (millisMain - waktuSebelum3) >= interval){
    waktuSebelum4 = millisMain;
    data2["callback-2"] = 1;
    data2["callback-3"] = 1;
    serializeJson(data2, Serial);
  }
}

void setup(void) 
{
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFISSID, PASSWORD);
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);  
  }
  
  while (!Serial) {
    // will pause Zero, Leonardo, etc until serial console opens
    delay(1);
  }
  antares.setDebug(true);
  antares.wifiConnection(WIFISSID,PASSWORD);
  antares.setMqttServer();
}

void loop(void) 
{
  unsigned long waktuSekarang = millis();
  DynamicJsonDocument data(500);
  if((unsigned long) (waktuSekarang - waktuSebelum1) >= 50) {
    waktuSebelum1 = waktuSekarang;
    sendCallback(waktuSekarang, 50);
    getData();
  }
  if((unsigned long) (waktuSekarang - waktuSebelum2) >= 3000) {
    waktuSebelum2 = waktuSekarang;
    antares.checkMqttConnection();
    // Add variable to data storage buffer
    antares.add("Tegangan", busvoltage*1000);
    antares.add("Arus", current_mA);
    antares.add("Daya", power_mW);
    
    // Publish and print data
    antares.publish(projectName, deviceName);
    // delay(3000);
  }
}
