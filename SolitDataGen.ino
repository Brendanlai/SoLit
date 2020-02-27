/*Arduino Temperature Voltage Data Logger with SD Mod
   https://randomnerdtutorials.com/arduino-temperature-data-logger-with-sd-card-module/
   To see the Schematics ^
*/

#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <DHT.h>
#include <DHT_U.h>

//Define DHT pin
#define DHTPIN 2 //connected pin

//Insert the type number we're using
#define DHTTYPE DHT11   // DHT 11 

//Initialize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

/*  Should change to match the SD shield or module
    Arduino Ethernet sheild/modules : pin4
    Datalogging SD shields/modules: pin10
*/
const int chipSelect = 4;

//Create file to store data
File myFile;

//RTC
RTC_DS1307 rtc;

void setup() {
  //initialize the DHT
  dht.begin();

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
  // Finding voltage from temperature reading
  // Read temp/humidty takes approx. 250 ms
  // Sensor readings may be up to 2 seconds old (slow sensor)
  // Read temp as celsius and convert to voltage
  float t = dht.readTemperature();
  float v = t * 5.0 / 1024.0;

  // Check if reads ahve failed and exit
  if (isnan(t)) { /*|| isnan(f)*/
    Serial.println("Failed to read from DHT");
    return;
  }

  //debugging
  Serial.print("Voltage");
  Serial.print(v);
  Serial.print("V");

  myFile = SD.open(("Data.txt"), FILE_WRITE);
  if (myFile) {
    Serial.println("open successfully");
    myFile.print(v);
    myFile.print(",");
  }
  myFile.close();
}

void loop() {
  // put your main code here, to run repeatedly:
  loggingTime();
  loggingVoltage();
  loggingVoltage();
}
