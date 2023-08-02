void pidSetup() {
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
  // t = millis();

  fc =0.23; //0.023
  PVf_1 = 0;
}

void pidMain() {
  Ts = 0.9;     //2;  0.9;   0.45;   //0.49
  Tsf = 1.2;    //2.2; 0.1;   0.08;   //0.15
  RC = 1/(6.28*fc);
  a = RC/Tsf;
  SV = analogRead(A0)*0.004887; 
  SVfix = SV*2; // 1 Volt = 2.8 cm
  
  // float uS = sonar.ping();
  // PV =10-(uS/US_ROUNDTRIP_CM);
  // PV = sonar.ping_cm();
  PV = jarakSensor(10);
  PVf = (PV + a*PVf_1)/(a + 1);
  PVfix = PVf; // kalibrasi sensor

  // t = millis();
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
}

float jarakSensor(float jarak_min) {
  float value = sonar.ping_result;
  float result = jarak_min - (value/US_ROUNDTRIP_CM);
  return result;
}