/*
_____IEEE Student Branch Miguel Hernandez of Elche University_____

Data-Logger Thermometer + Barometer to SDCard using Arduino unplugged. It is possible to debug using Ctrl+M (showing data in real time on SerialMonitor).
Sketch adapted by Javier Andres Perez. Originally extracted from the Official Arduino Reference ( http://arduino.cc/en/Tutorial/Datalogger ).

Thermometer implemented using a LM35 sensor. Schematics from front view: (5V, data to analog0, GND).
Barometer implemented using a X? sensor. Schematics from front view: (X?)

Using an Arduino UNO + Arduino Ethernet Shield (also possible to use a SD adapter)

_____IEEE Student Branch Miguel Hernandez of Elche University_____

Repo on GitHub: https://github.com/IEEEsbUMH/ArduinoStratosphericBalloon
 */

#include <SD.h> //Using SD Library to write data.


int sensortemp=0; //temperature sensor connected to analog0
float power=5.0;      //5V power
float analogdivisions=1024.0;   //number of steps on analogRead measures (values from 0 to 1024)
int vectortempsize=100;
float vectortemperature[vectortempsize];  //vector to store measures (optional)
float temperature;
float pressure;

int secondsON=0;
int hours=0;
int minutes=0;
int seconds=0;
int counter=0; //Counting fields (identifier)
int interval=5000; //Interval in ms between two measures

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.print("Inicializando tarjeta SD...");
  pinMode(10, OUTPUT);
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Fallo en la tarjeta o no disponible");
    // don't do anything more:
    return;
  }
  
  
  Serial.println("tarjeta inicializada.");
  
  File dataFile = SD.open("dataloggerstratosphericballoon.csv", FILE_WRITE);
  //If there are no problems initializing the SD Card, we create the header to be attached to previous records.
   if (dataFile) {
    dataFile.print("Stratospheric Balloon Project");
    dataFile.print(";");
    dataFile.println("IEEE Student Branch UMH");
    dataFile.println("");
    dataFile.print("ID");
    dataFile.print(";");
    dataFile.print("Time (h:m:s)");
    dataFile.print(";");
    dataFile.println("Temperature ºC");
    dataFile.close();
    delay(interval); //Wait an interval between two measures
}}

void loop() {
  
  secondsON=millis()/1000; //Using millis to know how much time elapsed since start
  hours=secondsON/3600;
  minutes=secondsON/60-hours*60;
  seconds=secondsON-minutes*60;
  
  //Filling the temperature vector with measures and conversion from tension to Celsius
  for(int i=0;i<vectortempsize;i++){
  vectortemperature[i] = analogRead(sensor);
  vectortemperature[i] = (power*vectortemperature[i]*100.0)/analogdivisions;  //LM 35 sensitivity (100)
  }
  for(int j=0;j<vectortempsize;j++){
  temperature=vectortemperature[j]+temperature;
  }
  temperature=temperature/vectortempsize;

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("dataloggerstratosphereballoon.csv", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(counter);
    dataFile.print(";");
    dataFile.print(hours);
    dataFile.print(":");
    dataFile.print(minutes);
    dataFile.print(":");
    dataFile.print(seconds);
    dataFile.print(";");
    dataFile.println(temperature);
    dataFile.close();
    // print on serial port:
    Serial.print(counter);
    Serial.print(";");
    Serial.print(hours);
    Serial.print(":");
    Serial.print(minutes);
    Serial.print(":");
    Serial.print(seconds);
    Serial.print(";");
    Serial.print(temperature);
    Serial.print((char)176);
    Serial.println("C");
    delay(intervalo); 
    counter++; //increasing the counter in each iteration
    
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error abriendo dataloggerstratosphereballoon.csv");
  } 
}

