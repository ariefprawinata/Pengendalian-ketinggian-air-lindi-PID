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
  // Hasil desain kendali
  // L = 0.1024431 dan T = 2.0803832
  Kp = 1;//20.3076;//5 0.374;
  Ti = 0;//0.30732;//0.1;//0.254;//0.374, 0.085;
  Td = 0;//0.05122;//0.021;

  if (Ti == 0){
    Ki = 0;
  }
  else{
    Ki = Kp/Ti;
  }
  Kd = Kp*Td;

  et_1 = 0;
  eint_1 = 0;
  interval_elapsed = 0;
  interval_limit = 2;
  t = millis();

  fc =0.23; //0.023
  PVf_1 = 0;
  
  Serial.begin(9600);
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
    Ts = 0.9;     //2;  0.9;   0.45;   //0.49
    Tsf = 1.2;    //2.2; 0.1;   0.08;   //0.15
    RC = 1/(6.28*fc);
    a = RC/Tsf;
    SV = analogRead(A0)*0.004887; 
    SVfix = SV*2; // 1 Volt = 2.8 cm
//    SVfix = 4.5;
//    PV = sonar.ping_cm();
    float uS = sonar.ping();
    PV =10-(uS/US_ROUNDTRIP_CM);
    PVf = (PV + a*PVf_1)/(a + 1);
//PVfix = (-2.64)*PVf + 38.28;
    PVfix = PVf; // kalibrasi sensor
//PVfix = 15.47 - PVf;
//PVfix = (-0.8)*PVf + 11.1;
//PVfix = PVf;
    t = millis();
    et = SVfix - PVfix;
    eint_update = ((et + et_1)*Ts)/2; //Rumus Luas Trapesium
    eint = eint_1 + eint_update;
    edif = (et - et_1)/Ts;
    PID = Kp*et + Ki*eint + Kd*edif;
    PID = PID/1;
    if(PID > 10){
      PID = 10;
    }
    else if(PID < 0){
      PID = 0;
    }
    else{
      PID = PID;
    }
    PID = PID/2;
    MV = PID*51;
    MV = SV*51;
    

  if (PV < SV){                      
  running = true;
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, MV);}
  else if (PV > SV){
  running = false;
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0);}
  
  //else if (PV == SV){
  //running = false;
  //digitalWrite(in1, LOW);
  //digitalWrite(in2, LOW);
  //analogWrite(enA, 0);
  //}
  //while(running = false){
  //digitalWrite(in1, HIGH);
  //digitalWrite(in2, LOW);
  //analogWrite(enA, MV);
  //}
    
    interval_elapsed = interval_elapsed + Ts;
    if(interval_elapsed >= interval_limit){
      Serial.print("20");
      Serial.print(" ");
      Serial.print("0");
      Serial.print(" ");
      Serial.print(SVfix);
      Serial.print(" ");
      Serial.println(PVfix);
     
//      Serial.print(" ");
//      Serial.println(MV);
    
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
}
