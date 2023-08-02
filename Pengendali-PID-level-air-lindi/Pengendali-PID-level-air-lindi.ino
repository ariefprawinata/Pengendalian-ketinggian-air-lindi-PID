#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>
#define TRIGGER_PIN  12
#define ECHO_PIN     11
#define MAX_DISTANCE 200

int enA = 10;
int in1 = 9;
int in2 = 8;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
LiquidCrystal_I2C lcd(0x27, 16, 2); //Alamat I2C yang digunakan 0x27. d
                                    //Alamat I2C yang lain 0x3F
//konfigurasi pin I2C
//SDA A4
//SCL A5t

float SV, SVfix, PV, PVf, PVf_1, PVfix, Kp, Ti, Td, Ki, Kd;
//float SVfix, PVfix;
float RC, a, fc;
float PID;
float et, et_1;
float eint, eint_1, eint_update;
float edif;
int MV;

unsigned long t;
double t_1, Ts, Tsf;
float interval_elapsed, interval_limit;

bool running = false;

void setup() {
  // put your setup code here, to run once:  
  Serial.begin(9600);
  pidSetup();
  lcd.backlight();
  lcd.init();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SV : ");
  lcd.setCursor(0,1);
  lcd.print("PV : ");
  lcd.setCursor(11,0);
  lcd.print("cm");
  lcd.setCursor(11,1);
  lcd.print("cm");

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  pidMain();
  if (PV < SV){                      
  running = true;
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, MV);
  }
  else if (PV > SV){
  running = false;
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0);
  }
  
  interval_elapsed = interval_elapsed + Ts;
  if(interval_elapsed >= interval_limit){
    Serial.print("20");
    Serial.print(" ");
    Serial.print("0");
    Serial.print(" ");
    Serial.print(SVfix);
    Serial.print(" ");
    Serial.println(PVfix);
    lcd.setCursor(5,0);
    lcd.print(SVfix);
    lcd.setCursor(5,1);
    lcd.print(PVfix);
    interval_elapsed = 0;
    t_1 = t;
    et_1 = et;
    eint_1 = eint;
  }
  PVf_1 = PVf;
  

//  PV = jarakSensor(10.8, 10);
//  Serial.print(PV);
//  Serial.println(" CM");0
}
