#include <LSD.h> 

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial); // wait for serial port to connect.
  Serial.print("Initializing SD card...");

  LSD.begin();

  Serial.println("card initialized.");
}

void loop()
{
  // make a string for assembling the data to log:
  String dataString = "";
  // read three sensors and append to the string:
  for (int analogPin = 14; analogPin < 17; analogPin++) {
    int sensor = analogRead(analogPin);
    dataString += String(sensor);
    if (analogPin < 16) {
      dataString += ",";
  }
}

// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another.
LFile dataFile = LSD.open("datalog.txt", FILE_WRITE);

// if the file is available, write to it:
if (dataFile) {
  dataFile.println(dataString);
  dataFile.close();
  // print to the serial port too:
  Serial.println(dataString);
}
// if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}
