/* IGEN230 Solit solar power collector
   Logging data from 2 analog sensors
   The Circuit:
      Analog sensors at pins 0 and 1 (can change this by chanign the i value in the for loop)

      SD card attached to SPI bus as follows: ??? Not sure about this part
    MOSI - pin 11
    MISO - pin 12
    CLK - pin 13
    CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
*/

#include <SPI.h>
#include <SD.h>


const int chipSelect = 10;
unsigned long time;

void setup() {
  // Open Serial Communications and wait for port to open:
  Serial.begin(9600);

  while (!Serial) {
    ;//Wait for serial port connection
  }

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card not present or failed!");
    while (1);
  }
  Serial.println("Card Initialized");

  delay(1000);
}

void loop() {
  // Create string for data logging:
  String toSD = "";
  String fileName = String(millis());
  fileName += ".txt";
  File dataFile = SD.open(fileName, FILE_WRITE);

  // Read three sensors and xappend to the string:
  for (int i = 0; i < 3; i++) {

    time = millis();
    
    if (i == 0) {
      toSD += time;
    } else {
      
      int sensVal = analogRead(i);
      // Convert the temp int val to voltage (0-5)V:
      float volt = sensVal * (5.0 / 1023);
      // Sends voltage data appending the float to string (4 decimals)
      toSD += String(volt, 3);
    
    }
    
    if ( i < 2) {
      toSD += ",";
    }

  }

  toSD += "\n";
  delay(1000);

  Serial.println(toSD);
  Serial.println(fileName);

  dataFile.println(toSD);
  dataFile.close();
  toSD = "";

  //serial.println("NEW FILE!");

}
