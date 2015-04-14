/*
#######################################################################################
                         IEEE SB UMH STRATOSPHERIC BALLOON.
#######################################################################################

Program for decode NMEA packets send by a GPS receiver connected via Serial Port.

This program is part of the code used in the proyect IEEE SB UMH Stratospheric Balloon.
and the code should be uploaded in the arduino master.

*/


#include <TinyGPS++.h> //Library for parse NMEA GPS packets
#include <SoftwareSerial.h> //Library which allow create a new Serial Port on arduino
#include <Wire.h> //Library used in I2C protocol

//Variables
static const int RXPin = 3, TXPin = 2; // TX and RX ports used by GPS
static const uint32_t GPSBaud = 9600; // GPS baudrate
float latitude; 
float longitude; 
String lati="0000.00";
String longi="0000.00";
char l[9]="0000.00N";
char ln[10]="00000.00W";
double t_anterior=0;


// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(115200); 
  ss.begin(GPSBaud);
  Wire.begin();                // Join i2c bus as master device
}
void loop()
{
  //To understand this see documentation about Tinygps++ Library. http://arduiniana.org/libraries/tinygpsplus/
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      if(millis()-t_anterior>=60000){
        sendInfo();
        t_anterior=millis(); 
      }
    }
  }
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring.")); // executed in case 
    while(true);
  }
}

//Function used to add leading zeros according to APRS NMEA format.
String floatToString( float n, int l, int d, boolean z){
 char c[l+1];
 String s;

 dtostrf(n,l,d,c);
 s=String(c);

 if(z){
 s.replace(" ","0");
 }

 return s;
}

//Function used to send GPS information to Slave Device 2 in APRS format.
void sendInfo(){
  //Uncomment the folllowing two lines to check the correct operation
  //char r;
    //Serial.println("llamada send");
    
  if (gps.location.isValid()){
    Serial.println("VALIDO");
  latitude=(int) gps.location.lat()*100+(gps.location.lat() - (int)gps.location.lat())*60;
      if(gps.location.lat()>0){
      lati=floatToString(latitude,7,2,true);
      lati+="N";
      lati.toCharArray(l,9);
      //Serial.println(l); //Uncomment this line to check
    }
    else{
      lati=floatToString(abs(latitude),7,2,true);
      lati+="S";
      lati.toCharArray(l,9);
    }
    longitude=(int)gps.location.lng()*100+(gps.location.lng() - (int)gps.location.lng())*60;
      if(gps.location.lng()>0){
      longi=floatToString(longitude,8,2,true);
      longi+="E";
      longi.toCharArray(ln,10);
      //Serial.println(gps.location.lng(),6); //Uncomment this line to check
    }
    else{
      longi=floatToString(longitude*-1,8,2,true);
      longi+="W";
      longi.toCharArray(ln,10);
      //Serial.println(ln); //Uncomment this line to check
    }
 }
  Wire.beginTransmission(2); //Starts transmission to Slave #2 device
  Wire.write(l);        // sends 8 bytes latitude information
  Wire.write(ln);              // sends 9 bytes longitude information
  Wire.endTransmission();  //close the connection with Slave #2 device
}
