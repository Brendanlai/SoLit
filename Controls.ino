/*
 Controls:
 BMS and Dimming Circuit
 
 BMS: Cuts the power supply once we have reached a minimum voltage out from the battery
 Dimming: Dependent on motion sesnor, light = Bright when motion detected, else dim
*/

int maxVolt = 12.6; //Indicates battery is at full capacity (100%)
int minVolt = 11.3; //Indicates Battery is at low capacity (10%)
int walkDelay = 10*1000; //time light is on high

int pinSoC = 1; // pin used for reading the state of charge (has to learn when to cut power out from the battery)
int pinBms = 2; // pin for the mosfet which triggers the charging
int pinSens1 = 3; //pin for the PIR sensor (detects left)
int pinSens2 = 4; //pin for the PIR sensor (detects right)
int pinLight = 13; //pin for the light
int pirState1, pirState2 = LOW; 

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600); 
}

/*Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):*/
int toVoltage(int sensorValue){
  float voltage = sensorValue * (5.0 / 1023.0);
  return voltage;
}

/* Checks if SOC is either too high or too low*/
void chargeState(int val){
  
  if (val > maxVolt || val < minVolt){
    digitalWrite(pinBms, LOW);
  }
 
  digitalWrite(pinBms, HIGH); 
}
/*Detects if there is motion, runs HIGH for "x" period of time before returning to LOW*/
void motionSensor(){
 
  pirState1 = analogRead(pinSens1);
  pirState2 = analogRead(pinSens2);
  
  do{
    digitalWrite(pinLight, HIGH);
  } while(pirState1 == HIGH || pirState2 == HIGH);
  
  //delay 5 extra seconds then turn off
  delay(2000);
  digitalWrite(pinLight, LOW);
}

/* Main Loop*/
void loop() {
 
  // read SOC pins
  int val0= analogRead(pinSoC); //state of charge
  
  toVoltage(val0);
  chargeState(val0);
  motionSensor();
}
