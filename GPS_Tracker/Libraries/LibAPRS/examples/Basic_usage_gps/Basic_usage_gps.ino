// Include LibAPRS
#include <LibAPRS.h>
#include <SoftwareSerial.h>

// Constants
#define txPin 12      //tx pin in GPS connection
#define rxPin 11     //rx pin in GPS connection

// You must define what reference voltage the ADC
// of your device is running at. If you bought a
// MicroModem from unsigned.io, it will be running
// at 3.3v if the "hw rev" is greater than 2.0.
// This is the most common. If you build your own
// modem, you should know this value yourself :)
#define ADC_REFERENCE REF_3V3
// OR
//#define ADC_REFERENCE REF_5V

// You can also define whether your modem will be
// running with an open squelch radio:
#define OPEN_SQUELCH false

// You always need to include this function. It will
// get called by the library every time a packet is
// received, so you can process incoming packets.
//
// If you are only interested in receiving, you should
// just leave this function empty.
// 
// IMPORTANT! This function is called from within an
// interrupt. That means that you should only do things
// here that are FAST. Don't print out info directly
// from this function, instead set a flag and print it
// from your main loop, like this:


// Include the SoftwareSerial library


// Set up the 
boolean gotPacket = false;
AX25Msg incomingPacket;
uint8_t *packetData;
void aprs_msg_callback(struct AX25Msg *msg) {
  // If we already have a packet waiting to be
  // processed, we must drop the new one.
  if (!gotPacket) {
    // Set flag to indicate we got a packet
    gotPacket = true;

    // The memory referenced as *msg is volatile
    // and we need to copy all the data to a
    // local variable for later processing.
    memcpy(&incomingPacket, msg, sizeof(AX25Msg));

    // We need to allocate a new buffer for the
    // data payload of the packet. First we check
    // if there is enough free RAM.
    if (freeMemory() > msg->len) {
      packetData = (uint8_t*)malloc(msg->len);
      memcpy(packetData, msg->info, msg->len);
      incomingPacket.info = packetData;
    } 
    else {
      // We did not have enough free RAM to receive
      // this packet, so we drop it.
      gotPacket = false;
    }
  }
}
SoftwareSerial gps = SoftwareSerial(rxPin, txPin);

// Variables
char byteGPS = 0;  
int i = 0;
int state = 0;
char dataGPG[100]="";
char *pch;
char *GGA[15];
int sat = 0;
void string()
{
  i=0;
  memset(GGA, 0, sizeof(GGA));          // Remove previous readings

  pch = strtok (dataGPG,",");

  // Analyze the saved interval in pch to see if it the needed string
  if (strcmp(pch,"$GPGGA")==0)
  {
    while (pch != NULL)
    {
      pch = strtok (NULL, ",");
      GGA[i]=pch;    
      i++;
    } 

    plot(GGA,"$GPGGA");         // Call to the function that is going to display the data
  }
}

/*
This function organize the gps data received for printing in the serial monitor.
 */

void plot(char **GGAPrint, char *trama)
{
  state = atoi(GGAPrint[5]);
  sat = atoi(GGAPrint[6]);

  if(trama=="$GPGGA" && state==1)
  {
    digitalWrite(13, HIGH);            // Then there are satellites, the LED switch ON

    Serial.println("");
    Serial.println(F("----------------------------------------------"));
    Serial.print(F("UTC Hour -> "));
    Serial.println(GGAPrint[0]);
    Serial.print(F("Latitude -> "));
    Serial.print(GGAPrint[1]);
    Serial.println(GGAPrint[2]);
    Serial.print(F("Longitude -> "));
    Serial.print(GGAPrint[3]);
    Serial.println(GGAPrint[4]);
    Serial.print(F("GPS quality: 0=null; 1=GPS fixed -> "));
    Serial.println(GGAPrint[5]);
    Serial.print(F("Number of satellites -> "));
    Serial.println(sat);
    Serial.print(F("Horizontal Dilution of Precision -> "));
    Serial.println(GGAPrint[7]);
    Serial.print(F("Antenna altitude -> "));
    Serial.print(GGAPrint[8]);
    Serial.println(GGAPrint[9]);
    Serial.print(F("Geoid Separation -> "));
    Serial.print(GGAPrint[10]);
    Serial.println(GGAPrint[11]);
    Serial.println(F("----------------------------------------------"));
    Serial.println("");

  }

  else                                // If no satellite connection
  {
    digitalWrite(13, LOW);                                              // Turn off the LED
    Serial.println("");
    Serial.println(F("-----------------------------"));
    Serial.print(F("|--- Satellites Used -->"));
    Serial.print(sat);
    Serial.println(" |");
    Serial.println(F("|----Waiting location----|"));
    Serial.println(F("-----------------------------"));
    Serial.println("");
  }
}

void setup() {
  // Set up serial port
  Serial.begin(9600);

  // Initialise APRS library - This starts the modem
  APRS_init(ADC_REFERENCE, OPEN_SQUELCH);

  // You must at a minimum configure your callsign and SSID
  APRS_setCallsign("NOCALL", 1);

  // You don't need to set the destination identifier, but
  // if you want to, this is how you do it:
  // APRS_setDestination("APZMDM", 0);

  // Path parameters are set to sensible values by
  // default, but this is how you can configure them:
  // APRS_setPath1("WIDE1", 1);
  // APRS_setPath2("WIDE2", 2);

  // You can define preamble and tail like this:
  // APRS_setPreamble(350);
  // APRS_setTail(50);

  // You can use the normal or alternate symbol table:
  // APRS_useAlternateSymbolTable(false);

  // And set what symbol you want to use:
  // APRS_setSymbol('n');

  // We can print out all the settings
  APRS_printSettings();
  Serial.print(F("Free RAM:     ")); 
  Serial.println(freeMemory());
 // pinMode(rxPin, INPUT);
  //pinMode(txPin, OUTPUT);
  gps.begin(9600);
  gps.flush();

  //setup satellites signal
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);     // Turn off the led until a satellite signal
}

void locationUpdateExample() {
  // Let's first set our latitude and longtitude.
  // These should be in NMEA format!
  APRS_setLat("5530.80N");
  APRS_setLon("01143.89E");

  // We can optionally set power/height/gain/directivity
  // information. These functions accept ranges
  // from 0 to 10, directivity 0 to 9.
  // See this site for a calculator:
  // http://www.aprsfl.net/phgr.php
  // LibAPRS will only add PHG info if all four variables
  // are defined!
  APRS_setPower(2);
  APRS_setHeight(4);
  APRS_setGain(7);
  APRS_setDirectivity(0);

  // We'll define a comment string
  char *comment = "LibAPRS location update";

  // And send the update
  APRS_sendLoc(comment, strlen(comment));

}

void messageExample() {
  // We first need to set the message recipient
  APRS_setMessageDestination("AA3BBB", 0);

  // And define a string to send
  char *message = "Hi there! This is a message.";
  APRS_sendMsg(message, strlen(message));

}

// Here's a function to process incoming packets
// Remember to call this function often, so you
// won't miss any packets due to one already
// waiting to be processed
void processPacket() {
  if (gotPacket) {
    gotPacket = false;

    Serial.print(F("Received APRS packet. SRC: "));
    Serial.print(incomingPacket.src.call);
    Serial.print(F("-"));
    Serial.print(incomingPacket.src.ssid);
    Serial.print(F(". DST: "));
    Serial.print(incomingPacket.dst.call);
    Serial.print(F("-"));
    Serial.print(incomingPacket.dst.ssid);
    Serial.print(F(". Data: "));

    for (int i = 0; i < incomingPacket.len; i++) {
      Serial.write(incomingPacket.info[i]);
    }
    Serial.println("");

    // Remeber to free memory for our buffer!
    free(packetData);

    // You can print out the amount of free
    // RAM to check you don't have any memory
    // leaks
    // Serial.print(F("Free RAM: ")); Serial.println(freeMemory());
  }
}

boolean whichExample = true;
void loop() {

  delay(1000);
  if (whichExample) {
    locationUpdateExample();
  } 
  else {
    messageExample();
  }
  whichExample ^= true;

  delay(500);
  processPacket();

  gps.flush();
  Serial.flush();
  Serial.println(F("Waiting GPS data..."));

  // Prepare all for reading GPS Serial Port
  memset(dataGPG, 0, sizeof(dataGPG));    // Remove previous readings
  byteGPS = ' ';                            // Remove data
  byteGPS = gps.read();    // Read the byte that is in the GPS serial port
  Serial.println(byteGPS);
  delay(1000);

  // Find the desired string
  while(byteGPS != '$')
  {
    byteGPS= gps.read();
    delay(500);
    Serial.println(byteGPS);
  } 

  // Save the string in an array
  i=1;
  dataGPG[0] = '$';

  while(byteGPS != '*' )
  {
    byteGPS = gps.read();
    dataGPG[i]=byteGPS; 
    i++; 
  }

  dataGPG[i]= '\0';
  string();                                 // Call to the function that manipulates our string


}


