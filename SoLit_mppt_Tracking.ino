/*  MPPT Solar Charger Arduino
 *  By: Brendan Lai adapted from https://create.arduino.cc/projecthub/abhi_verma/arduino-pv-mppt-solar-charger-371474
 *  See Trello for documentation
 *  Uses P&O algorithm to regulate the charge voltage being sent from the panel to the battery to maximize.
 *  IGEN230
 */

// #include <LiquidCrystal.h>
// #include <avr/wdt.h>
// #include <EEPROM.h>
#define vin_pin A1
#define vout_pin A0
#define iout_pin A2
#define iin_pin A3
#define lm35 A4
#define fan 5
#define buck_pin 6
#define menu 3
#define button 2
#define Bat 13
#define charge_Bat A5
#define light 4

uint8_t auto_mode= 1;
float Pin=0,Pout=0,Pin_previous=0;
float efficiency=0.0;
int raw_vin, raw_vout, raw_iout,raw_iin, raw_lm35=0;
float Vout_boost = 18.3,Vout_max = 20.0, Iout_max = 2.9, Vout_float = 13.5, Iout_min = 0.00, Vin_thresold = 15.0;
float Iout_sense,Iin_sense,Iin;
float Vout_sense,Vin_last;
float heat_sink_temp;
float Vin_sense;
uint8_t duty_cycle = 0;
float volt_factor = 0.05376; //change this value to calibrate voltage readings...
String mode="";
bool startup=true, lcd_stat=true,charge=true,mppt_init = true;
unsigned int count=0;

void mppt();
void setup() {
 
  Serial.begin(115200);
  pinMode(light,OUTPUT);
  pinMode(charge_Bat,OUTPUT);
  digitalWrite(charge_Bat,LOW);
  digitalWrite(light,HIGH);
  pinMode(Bat,OUTPUT);
  analogWrite(buck_pin,0);
 
  for(int i=0;i<10;i++) {
    raw_iout += analogRead(iout_pin)-513;
    raw_iin += analogRead(iin_pin)-513;
    raw_vin += analogRead(vin_pin);
    raw_vout += analogRead(vout_pin);
    raw_lm35 += analogRead(lm35);
    delay(2);
  }

  raw_iout = raw_iout / 10;
  raw_iin = raw_iin / 10;
  raw_vout = raw_vout / 10;
  raw_vin = raw_vin / 10;
  Iout_sense = float(raw_iout) *5/1023/0.066;
  Iin_sense = float(raw_iin) *5/1023/0.066;

  Vin_sense = float(raw_vin) * volt_factor;
 
}

////This function provides various regulations and MPPT implementation...
void regulate(float Iout, float Vin, float Vout) {
  mode="";
  mode="Buck mode";

  if((Vout>Vout_max) || (Iout>Iout_max) || ((Pin>Pin_previous && Vin_sense<Vin_last) || (Pin<Pin_previous && Vin_sense>Vin_last))) {
    if(duty_cycle>0) {
    duty_cycle-=1;
    }
    analogWrite(buck_pin,duty_cycle);
  } else if((Vout<Vout_max) && (Iout<Iout_max) && ((Pin>Pin_previous && Vin_sense>Vin_last) || (Pin<Pin_previous && Vin_sense<Vin_last))) {
      if(duty_cycle<250) {
      duty_cycle+=1;
    }
    analogWrite(buck_pin,duty_cycle);
  }
  
  Pin_previous = Pin;
  Vin_last = Vin;
}

void auto_cutoff(float Iout,float Vin, float Vout){
  
  if(Vout<=Vout_float && Iout>Iout_min+1){
    charge = true;
  }
  
  if((Vout>Vout_max) && (Iout<Iout_min) && (charge==true)) {
    
    charge = false;
    Serial.println("Charging Completed.");
    digitalWrite(Bat,HIGH);
    digitalWrite(charge_Bat,LOW);

    } else if(Vin<Vin_thresold) {
      
      duty_cycle=0;
      analogWrite(buck_pin,duty_cycle);
      Serial.println("LOW Input Voltage.");
      
      for(int i=0;i<10;i++){
        digitalWrite(Bat,HIGH);
        digitalWrite(charge_Bat,LOW);
        delay(6000);
        digitalWrite(charge_Bat,HIGH);
        digitalWrite(Bat,LOW);
        delay(6000);
    }

  } else if(heat_sink_temp>80.0) {
    
    duty_cycle=0;
    analogWrite(buck_pin,duty_cycle);
    Serial.println("Over Heat Shutdown");

    for(int i=0;i<10;i++){
      digitalWrite(Bat,HIGH);
      digitalWrite(charge_Bat,LOW);
      delay(4000);
      digitalWrite(charge_Bat,HIGH);
      digitalWrite(Bat,LOW);
      delay(4000);
    }

  } else {
    charge = true;
    digitalWrite(charge_Bat,HIGH);
    regulate(Iout_sense, Vin_sense, Vout_sense);
    digitalWrite(Bat,LOW);
  }
}

void soft_start() {
  for(int i=0;i<20;i++) {
    regulate(Iout_sense, Vin_sense, Vout_sense);
    Serial.print("Vin= ");Serial.println(Vin_sense);
    Serial.print("Vout= ");Serial.println(Vout_sense);
    Serial.print("Iout= ");Serial.println(Iout_sense);
    Serial.print("Duty cycle= ");Serial.println(duty_cycle);
    Serial.print("Charger MODE : ");Serial.println(mode);
    Serial.println("Soft Start Activated");
    delay(32000);
  }
 
  startup=false;
  mppt_init = false;
}

void set_limits(int cmd,int temp){
  switch(cmd) {
    case 1:
    Vout_boost=float(temp)/10;
    Serial.print("Vout_boost= ");
    Serial.println(Vout_boost);
    break;
    case 2:
    Vout_float=float(temp)/10;
    Serial.print("Vout_float= ");
    Serial.println(Vout_float);
    break;
    case 3:
    Iout_max=float(temp)/10;
    Serial.print("Iout_max= ");
    Serial.println(Iout_max);
    break;
    case 4:
    Iout_min=float(temp)/10;
    Serial.print("Iout_min= ");
    Serial.println(Iout_min);
    break;
    case 5:
    auto_mode=~auto_mode;
    if(auto_mode==1){
    Serial.println("Auto Mode ON    ");
    }
    else {
    Serial.println("Auto Mode OFF   ");
    }
    break;
    case 6:
    Vin_thresold=float(temp)/10;
    Serial.print("Vin_thresold= ");
    Serial.println(Vin_thresold);
    break;
  }
}

void loop() {

  if(charge){
    Vout_max = Vout_boost;
  } else {
    Vout_max = Vout_float;
  }
  
  raw_vin, raw_vout, raw_iout, raw_lm35 = 0;
 
  if(Serial.available()>0) {
    String data = Serial.readString();
    int temp = data.toInt();
    int func = temp%10;
    temp = temp/10;
    set_limits(func,temp);
  }
  
  
  for(int i = 0; i < 10; i++){
    raw_iout += analogRead(iout_pin)-513;
    raw_iin += analogRead(iin_pin)-513;
    raw_vin += analogRead(vin_pin);
    raw_vout += analogRead(vout_pin);
    raw_lm35 += analogRead(lm35);
    delay(1);
  }

  raw_iout = raw_iout / 10;
  raw_iin = raw_iin / 10;
  raw_vout = raw_vout / 10;
  raw_vin = raw_vin / 10;
  raw_lm35 = raw_lm35 / 10;

  Iout_sense = float(raw_iout) *5/1023/0.066;
  Iin_sense = float(raw_iin) *5/1023/0.066;
  Vin_sense = Vin_sense * 0.92 + float(raw_vin) * volt_factor * 0.08;
  Vout_sense = Vout_sense * 0.92 + float(raw_vout) * volt_factor * 0.08; //measure output voltage.
  
  if(Iout_sense<0.0){
    Iout_sense= Iout_sense*(-1);
  }

  if(Iin_sense<0.0){
    Iin_sense = Iin_sense*(-1);
  } 

  Pin = Vin_sense*Iin_sense;
  Pout = Vout_sense*Iout_sense;
  efficiency = Pout*100/Pin;
  
  if(efficiency<0.0){
    efficiency=0.0;
  }

  if(count>100) {
    Serial.print("heat_sink_temp = "); Serial.println(heat_sink_temp);
    Serial.print("Raw= ");Serial.println(raw_iout);
    Serial.print("Vin= ");Serial.println(Vin_sense);
    Serial.print("Iin= ");Serial.println(Iin_sense);
    Serial.print("Vout= ");Serial.println(Vout_sense);
    Serial.print("Iout= ");Serial.println(Iout_sense);
    Serial.print("Duty cycle= ");Serial.print(duty_cycle/2.55);Serial.println("%");
    Serial.print("PV power = ");Serial.println(Pin);
    Serial.print("Output power = ");Serial.println(Pout);
    Serial.print("Efficiency = ");Serial.print(efficiency);Serial.println("%");
    Serial.print("Converter MODE : ");Serial.println(mode);
 
    digitalWrite(Bat,HIGH);
    delay(16000);
    count = 0;
  }

  if(startup==false) {
    
    if(auto_mode==1){
      auto_cutoff(Iout_sense,Vin_sense, Vout_sense);
    } else {
      digitalWrite(charge_Bat,HIGH);
      regulate(Iout_sense,Vin_sense, Vout_sense);
      digitalWrite(Bat,~digitalRead(Bat));
    }
  }
  
  if(heat_sink_temp>45.0){
    digitalWrite(fan,HIGH);
  } else if (heat_sink_temp<37.0) {
    digitalWrite(fan,LOW);
  }

  count++;

}

