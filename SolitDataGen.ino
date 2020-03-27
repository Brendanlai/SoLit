/*Arduino Temperature Voltage Data Logger with SD Mod
   https://randomnerdtutorials.com/arduino-temperature-data-logger-with-sd-card-module/
   To see the Schematics ^
*/

#include <SPI.h>
#include <SD.h>
#include <RTClib.h>


/*  Should change to match the SD shield or module
    Arduino Ethernet sheild/modules : pin4
    Datalogging SD shields/modules: pin10
*/
const int chipSelect = 4;
const int numPin = 2;

//Create file to store data
File myFile;

//RTC
RTC_DS1307 rtc;

void setup() {
  //initialize the DHT
//   dht.begin();

  // initialize serial monitor
  Serial.begin(9600);

  // setup RTC
  while (!Serial);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  } else {
    //sets RTC to the date&time of the sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC NOT running");
  }

  // setup SD Card
  Serial.print("Initializing SD");

  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization failed");
    return;
  }
  Serial.println("Completed initialization");

  // if file correctly opened, write to it:
  if (myFile) {
    Serial.println("File opened ok");
    //print data headings
    myFile.println("Date,Time,V1,V2");
  }
  myFile.close();
}

void loggingTime() {
  DateTime now = rtc.now();
  myFile = SD.open("Data.txt", FILE_WRITE);
  if (myFile) {
    myFile.print(now.year(), DEC);
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print(',');
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    myFile.print(now.second(), DEC);
    myFile.print(",");
  }
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.println(now.day(), DEC);
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
  myFile.close();
  delay(1000);
}

void loggingVoltage() {
  // Open File
  myFile = SD.open(("Data.txt"), FILE_WRITE);
  if (myFile) {
    Serial.println("open successfully");
  }

  // Continue loop to capture data for set amount of pins
  // Loop starts at 0 and thus pin must connect to pin 0 increasing to whatever you want to end with
  for (int i = 0; i < numPin; i++) {

    // Record from pin and convert the temp reading to voltage.
    int t = analogRead(i);
    float v = t * 5.0 / 1023.0;
    myFile.print(v);
    myFile.print(",");

    //debugging
    Serial.print("Voltage");
    Serial.print(v);
    Serial.print("V");
  }
  myFile.println();
  myFile.close();
}


void loop() {
  // put your main code here, to run repeatedly:
  loggingTime();
  loggingVoltage();
}
