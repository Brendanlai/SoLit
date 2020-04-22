/*  Perturb and Observe Algorithm Implementation
    Used for tracking Max. power abs. from Mppt. curve
*/

#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

  const int pwmUp = 10;     // pin select for actuator extend
  const int pwmDown = 6;    // pin select for actuator retract
  const int onPwm = 125; //Requires testing
  const int adjustTime = 50 // Actuator moves at 1.38 inches/second = 35 mm/second, want extend by 4 deg each time

  const int mode = 1; // Set this based on the direction desired (if light is facing west mode = 1)
  
  const int chipSelect = 4; //SD pin
  int fill; //Fill these vals
  int v_in = A0, i_in = A3, pwm_pin = 9; // the input and output pins
  long timeDiff, prevTime = 0;

  File myFile;
  RTC_DS1307 rtc;

void setup(){
  // For actuator
  pinMode(pwmUp, OUTPUT);
  pinMode(pwmDown, OUTPUT);
  Timer1.pwm(pwmUp, D);
  Timer1.pwm(pwmDown, D);
  Serial.begin(9600);
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

/*Functions*/
void retract(){
  while(time(millis)
  digitalWrite(pwmDown, onPwm);
  delay(adjustTime);
}
  
void extend(){
  digitalWrite(pwmUp, onPwm);
  delay(adjustTime);
  }

void loop(){
  while(now.hour > 07 && now.hour < 19){
    while((now.second ==  00 && now.minute ==00) || (now.second == 00 && now.minute == 30)){
      if(mode == 0){
        retract();
      }else if(mode == 1){
        extend();
      }
    }
  }
  if(mode == 0){
    extend();
  } else {
    retratct();
  }
}
