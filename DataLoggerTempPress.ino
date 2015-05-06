/*
Datalogger for the Stratospheric Balloon Project (IEEE Student Branch Universidad Miguel Hernandez of Elche)
Using SD card, BMP180 and DS18B20 examples adapted (that's why there are some code in spanish and some in english).

Datalogger para el proyecto del Globo Estratosferico (IEEE Student Branch Univ. Miguel Hernandez de Elche).
Usamos los ejemplos adaptados para la tarjeta SD y los sensores BMP180 y DS18B20 (por eso hay trozos de codigo en español y otros en ingles).
*/
#include <SD.h>
#include <OneWire.h>
#include <SFE_BMP180.h>
#include <Wire.h>

SFE_BMP180 pressure;

#define ALTITUDE 80.0 // Altitude of our headquarters (IEEE SB UMH) in Elche, SPAIN

//To avoid memory troubles we declare the variables here, and not in the loop as suggested in the examples. There might be problems opening the FILE if we declare variables in every execution.
  char status;
  double T,P,p0,a;
  double presion;
  float temperature1;
  
  byte data[12];
  byte addr[8];
  byte present;
  
  byte MSB;
  byte LSB;
  float CONT_REMAIN;
  float CONT_PER_C;
  
  float tempRead;
  float TemperatureSum;

const int chipSelect = 10;
int DS18B20_Pin = 2; //DS18B20 Signal pin on digital 2
int pin_led=3;
int parpadeo=0;
//Temperature chip i/o
OneWire ds(DS18B20_Pin); // on digital pin 2
int segundosencendido=0;
int horas=0;
int minutos=0;
int segundos=0;
int conta=0; //ID de registros
int intervalo=5000; //Intervalo entre muestras (en milisegundos)

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.print("Inicializando tarjeta SD...");
  pinMode(10, OUTPUT);
  pinMode(3,OUTPUT);
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Fallo en la tarjeta o no disponible");
    // don't do anything more:
    return;
  }
  Serial.println("tarjeta inicializada.");
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
   if (dataFile) {
    dataFile.print("IEEE Student Branch UMH");
    dataFile.print(";");
    dataFile.println("www.umh.ieeespain.org");
    dataFile.println("");
    dataFile.print("Nº registro");
    dataFile.print(";");
    dataFile.print("Tiempo h:m:s");
    dataFile.print(";");
    dataFile.print("Temperatura1 ºC");
    dataFile.print(";");
    dataFile.print("Temperatura2 ºC");
    dataFile.print(";");
    dataFile.print("Presion (mBar)");
    dataFile.print(";");
    dataFile.println("Altitud (metros)");
    dataFile.close();
    delay(intervalo);
}
if (pressure.begin())
    Serial.println("Sensor de temperatura inicializado");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("Error inicializando sensor de temperatura.\n\n");
    while(1); // Pause forever.
  }
  
  pinMode(pin_led,OUTPUT);
}

void loop() {
  
  segundosencendido=millis()/1000;
  horas=segundosencendido/3600;
  minutos=segundosencendido/60-horas*60;
  segundos=segundosencendido-minutos*60;
    
temperature1 = getTemp();

presion=getPress();
p0 = pressure.sealevel(P,ALTITUDE); // we're at 80 meters (Elche, SPAIN)
a = pressure.altitude(P,p0);


  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.csv", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    //Hay un LED que se enciende y apaga para comprobar que esta abriendo bien el archivo datalog.csv
    //Serial.println("Funciona el dataFile");
    if(parpadeo==0){
    digitalWrite(pin_led,HIGH);
    dataFile.print(conta);
    dataFile.print(";");
    dataFile.print(horas);
    dataFile.print(":");
    dataFile.print(minutos);
    dataFile.print(":");
    dataFile.print(segundos);
    dataFile.print(";");  
    dataFile.print(temperature1); //midiendo la temperatura del sensor de temperatura
    dataFile.print(";");    
    dataFile.print(T-3);  //midiendo la temperatura del BMP
    dataFile.print(";");
    dataFile.print(presion);
    dataFile.print(";");
    dataFile.println(a);
    dataFile.close();
    parpadeo=1;
    }
    
    else{
      digitalWrite(pin_led,LOW);
    dataFile.print(conta);
    dataFile.print(";");
    dataFile.print(horas);
    dataFile.print(":");
    dataFile.print(minutos);
    dataFile.print(":");
    dataFile.print(segundos);
    dataFile.print(";");
    dataFile.print(temperature1); //midiendo la temperatura del sensor de temperatura
    dataFile.print(";");
    dataFile.print(T);  //midiendo la temperatura del BMP
    dataFile.print(";");
    dataFile.print(presion);
    dataFile.print(";");
    dataFile.println(a);
    dataFile.close();
    parpadeo=0;
    }
    // imprimir en serial para hacer debug:
    /*
    Serial.print(conta);
    Serial.print(";");
    Serial.print(horas);
    Serial.print(":");
    Serial.print(minutos);
    Serial.print(":");
    Serial.print(segundos);
    Serial.print(";");
    Serial.print(temperature);
    Serial.print((char)176);
    Serial.print("C");
    Serial.print(";");
    Serial.println(presion);
    */
    delay(intervalo); 
    conta++; //incrementa el contador
    
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error abriendo datalog.csv");
  } 
}

double getPress(){
  //devuelve la presion del sensor
  // If you want to measure altitude, and not pressure, you will instead need
  // to provide a known baseline pressure. This is shown at the end of the sketch.

  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    
    status=1;
    if (status != 0)
    {
      // Print out the measurement:
      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.print(" deg C, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" deg F");
      
      
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          Serial.println("");
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.println(" mb, ");
         }
        else Serial.println(F("error retrieving pressure measurement\n")); //Serial.println(F("texto")); se usa para cargar el mensaje en flash y no RAM
      }
      else Serial.println(F("error retrieving pressure measurement\n"));
    }
    else Serial.println(F("error retrieving pressure measurement\n"));
  }
  else Serial.println(F("error retrieving pressure measurement\n"));
  return(P);
}

float getTemp(){
//returns the temperature from one DS18B20 in DEG Celsius

if ( !ds.search(addr)) {
//no more sensors on chain, reset search
   ds.reset_search();
return -1000;
}

if ( OneWire::crc8( addr, 7) != addr[7]) {
Serial.print("CRC no valido!\n");
return -1000;
}

if ( addr[0] != 0x10 && addr[0] != 0x28) {
Serial.print("Dispositivo no reconocido");
return -1000;
}

ds.reset();
ds.select(addr);
ds.write(0x44,1); // start conversion, with parasite power on at the end

present = ds.reset();
ds.select(addr);  
ds.write(0xBE); // Read Scratchpad


for (int i = 0; i < 12; i++) { // resolucion de 12 bytes
  data[i] = ds.read();
}

ds.reset_search();

MSB = data[1];
LSB = data[0];
CONT_REMAIN = data[6];
CONT_PER_C = data[7];

tempRead = ((MSB << 8) | LSB); //using two's compliment 

TemperatureSum = tempRead/16;

return TemperatureSum;

}

