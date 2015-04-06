#include <TinyGPS++.h>
#include <SoftwareSerial.h>
/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
 It requires the use of SoftwareSerial, and assumes that you have a
 4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
 */
static const int RXPin = 3, TXPin = 2;
static const uint32_t GPSBaud = 9600;
float latitude;
float longitude;
char lati[9];
char longi[12];
double t_actual=0;
double t_anterior=0;
// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); 
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      if(Serial.read()=='L'){
        displayInfo();
      }
    }
  }
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}


void displayInfo()
{
  Serial.print(F("L")); 
  if (gps.location.isValid())
  {
    latitude=gps.location.lat()*100+(gps.location.lat() - (int)gps.location.lat())*60;
    dtostrf(latitude,9, 2, lati);
    Serial.print(lati);
    if(gps.location.lat()>0){
      Serial.print(F("N"));
    }
    else{
      Serial.print(F("S"));
    }
    Serial.print(F(","));
    longitude=gps.location.lng()*100+(gps.location.lng() - (int)gps.location.lng())*60;

    padding(abs((int)gps.location.lng()),4);
    if(gps.location.lng()>0){
      Serial.print(longitude);
      Serial.print(F("E"));
    }
    else{
      Serial.print(longitude*-1);
      Serial.print(F("W"));
    }
    Serial.print(F("*"));
  }


  else
  {
    Serial.print(F("INVALID*"));
  }

  /*Serial.print(F("  Date/Time: "));
   if (gps.date.isValid())
   {
   Serial.print(gps.date.month());
   Serial.print(F("/"));
   Serial.print(gps.date.day());
   Serial.print(F("/"));
   Serial.print(gps.date.year());
   }
   else
   {
   Serial.print(F("INVALID"));
   }
   
   Serial.print(F(" "));
   if (gps.time.isValid())
   {
   if (gps.time.hour() < 10) Serial.print(F("0"));
   Serial.print(gps.time.hour());
   Serial.print(F(":"));
   if (gps.time.minute() < 10) Serial.print(F("0"));
   Serial.print(gps.time.minute());
   Serial.print(F(":"));
   if (gps.time.second() < 10) Serial.print(F("0"));
   Serial.print(gps.time.second());
   Serial.print(F("."));
   if (gps.time.centisecond() < 10) Serial.print(F("0"));
   Serial.print(gps.time.centisecond());
   }
   else
   {
   Serial.print(F("INVALID"));
   }*/

  Serial.println();
}

void padding( int number, byte width ) {
  int currentMax = 10;
  for (byte i=1; i<width; i++){
    if (number < currentMax) {
      Serial.print("0");
    }
    currentMax *= 10;
  }
} 

