#include <Wire.h>
#include <Adafruit_INA219.h>
#include <ArduinoJson.h>

#define TRANSISTOR_PIN 4  // Ganti dengan pin yang sesuai
#define VOLTAGE_THRESHOLD 1000 // Tegangan dalam mV (1V = 1000mV) 

Adafruit_INA219 ina219;

float busvoltage = 0, current_mA = 0, power_mW = 0;

int callback2 = 0, callback3 = 0;
unsigned long waktuSebelum1 = 0, waktuSebelum2 = 0, waktuSebelum3 = 0, waktuSebelum4 = 0;

void setup(void) 
{
  pinMode(TRANSISTOR_PIN, OUTPUT);
  digitalWrite(TRANSISTOR_PIN, LOW);  // Matikan transistor awalnya

  Serial.begin(115200);
  Serial3.begin(9600);
  while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }

   uint32_t currentFrequency;
  
  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();
}

void loop(void) 
{
  DynamicJsonDocument data(500);

  unsigned long waktuSekarang = millis();
  if((unsigned long)(waktuSekarang - waktuSebelum1) >= 1000){
    waktuSebelum1 = waktuSekarang;
    busvoltage = ina219.getBusVoltage_V();
    current_mA = ina219.getCurrent_mA();
    power_mW = ina219.getPower_mW();
    Serial.print("Bus Voltage: "); Serial.print(busvoltage); Serial.println(" V");
    Serial.print("Current: "); Serial.print(current_mA); Serial.println(" mA");
    Serial.print("Power: "); Serial.print(power_mW); Serial.println(" mW");
  }

  if((unsigned long)(waktuSekarang - waktuSebelum2) >= 500) {
    waktuSebelum2 = waktuSekarang;
    if (busvoltage < VOLTAGE_THRESHOLD) {
      Serial.println ("ON");
    digitalWrite(TRANSISTOR_PIN, HIGH); // Hidupkan transistor
    }else {
    Serial.println ("OFF");
    digitalWrite(TRANSISTOR_PIN, LOW); // Matikan transistor
    }
    delay(3000);
  }
    

  if((unsigned long)(waktuSekarang - waktuSebelum3) >= 50){
    waktuSebelum3 = waktuSekarang;
    sendCallback(); 
    getCallback();
    if(callback2 == 1){
      sendData(busvoltage, current_mA, power_mW, waktuSekarang, 50);
    }
  }
}

void sendCallback() {
  DynamicJsonDocument data(500);
  data["callback-1"] = 1;
  serializeJson(data, Serial3);
}

void sendData(float busvoltage, float current_mA, float power, long millisMain, long interval) {
  DynamicJsonDocument data(500);
  if((unsigned long)(millisMain - waktuSebelum4) >= interval) {
    waktuSebelum4 = millisMain;
    if(callback3 == 1) {
      data["busvoltage"] = busvoltage;
      data["current"] = current_mA; 
      data["power"] = power;
      data["callback-4"] = 1;
      serializeJson(data, Serial3);
    }
  }
}

void getCallback() {
  DynamicJsonDocument data(500);
  deserializeJson(data, Serial3);
  int callback2 = data["callback-2"];
  int callback3 = data["callback-3"];
}
