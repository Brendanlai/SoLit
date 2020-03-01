/*  Perturb and Observe Algorithm Implementation
    Used for tracking Max. power abs. from Mppt. curve


*/

#include <TimerOne.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

  const int chipSelect = 4; //SD pin
  int fill; //Fill these vals
  int v_in = A0, i_in = A3, pwm_pin = 9; // the input and output pins
  int samples = 20;
  double Vpv, Ipv, Ppv, V_pv, Vpv_old, Ppv_old = 0;
  double dV, dP, err, P_PI, I_PI = 0.0;
  double Vref, Vref_old = fill , del_V = fill;
  double Vref_ul = fill, Vref_ll = fill;
  double Kp = 0.138, Ki = 15.173;
  int D = 512, D_old = 512;
  double D_temp, D_temp_old = 0;
  long timeDiff, prevTime = 0;

  File myFile;
  RTC_DS1307 rtc;
  

void setup(){
  // For actuator
  pinMode(pwm_pin, OUTPUT);
  Timer1.initialize(20);
  Timer1.pwm(pwm_pin, D);
  Serial.begin(9600);
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  if (myFile) {
    Serial.println("File opened ok");
    //print data headings
    myFile.println("Date,Time,V1,V2");
  }
  myFile.close();
}

/*Functions*/

// Reading input signal from analog pin (in temp
int pinRead(int pinNum){
  unsigned long int sum = 0;
  int x = 0;

  for(int i = 0; i < samples; i++){
    x = analogRead(pinNum);
    sum += x;
    delay(1);
  }
  int avg = sum / samples;

  return(avg);
}

// Reading inputs and converting analog values
void readData(){
  Vpv = pinRead(v_in) * 11.0 * (5.0/1023.0);
  Ipv = pinRead(i_in) * (5.0/1023.0);
  Ppv = Vpv * Ipv;
}

// P & O - MPPT alogrithm using Vref
void PnO(){
  dV = Vpv- Vpv_old;
  dP = Ppv - Ppv_old;
  if (dP > 0.0){
    if (dV > 0.0){
       Vref = Vref_old + del_V;     
    }else{
      Vref = Vref_old - del_V;
    }
  }else{
    if (dV < 0.0){
      Vref = Vref_old - del_V;
    }else{
       Vref = Vref_old + del_V;
    }
  }
  range();
}

// Keep Vref within the correct range
void range(){
  if ((Vref > Vref_ul) || (Vref< Vref_ll)){
    Vref = Vref_old;
  }
}

// generate PWM pulses 
void pwmDuty(){
  err = Vpv - Vref;
  P_PI = Kp * err; //Proportionality

  timeDiff = ((millis() - prevTime))/1000;
  I_PI += (Ki * err) * timeDiff; //integral term

  prevTime = millis();

  D_temp = P_PI + I_PI; //generated duty cycle

  D = abs(D_temp * 1023); //Converting pwm scale of 0-1023

  if (D > 900 || D < 100){
    D = D_old;
  }
  Timer1.pwm(pwm_pin, D);
}

// Recording data obtained
void dataSD(){
  DateTime now = rtc.now();
  myFile = SD.open(("Data.txt"), FILE_WRITE);
  
  if (myFile) {
    Serial.println("open successfully");
    
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    myFile.print(now.second(), DEC);
    myFile.print(",");

    myFile.print(Vpv);
    myFile.print(",");
    myFile.print(Ipv);
    myFile.print(",");
    myFile.print(Vref);
    myFile.print(",");
    myFile.print(Ppv);
    myFile.print(",");
    myFile.print(D);
    myFile.print(",");
    myFile.println(D_temp);
  }
  
}

// Save data for current iteration
void saveData(){
  D_old = D;
  D_temp_old = D_temp;
  Vpv_old = Vpv;
  Ppv_old = Ppv;
  Vref_old = Vref;
 }

void loop(){
  readData();
  PnO();
  pwmDuty();
  dataSD();
  saveData();
  delay(500);
}
