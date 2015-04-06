/*
#######################################################################################
                         IEEE SB UMH STRATOSPHERIC BALLOON.
#######################################################################################
Program that creates APRS packet for analogic TLC modem. Based in BasicUsage.ino example
of the library LibAPRS.
This program is part of the code used in the proyect IEEE SB UMH Stratospheric Balloon.
and the code should be uploaded in the arduino Slave #2.
*/

#include <Wire.h>
#include <LibAPRS.h> // Include LibAPRS


// You must define what reference voltage the ADC
// of your device is running at.
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

//Variables
boolean gotPacket = false;
char cad[100]="";
char *data[20];
char *pch;
char dataRead=' ';
int i;
char *comment;
char latitud[9]="0000.00N";
char longitud[10]="00000.00W";
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

void setup() {
  // Set up serial port
  Serial.begin(115200);
  Wire.begin(2);     //Starts as Slave #2
  Wire.onReceive(receiveEvent);   //Call function receiveEvent() when receive a packet from Master device
  comment=" ";
  // Initialise APRS library - This starts the modem
  APRS_init(ADC_REFERENCE, OPEN_SQUELCH);

  // You must at a minimum configure your callsign and SSID
  APRS_setCallsign("UMHBAL", 1);

  // You don't need to set the destination identifier, but
  // if you want to, this is how you do it:
  // APRS_setDestination("APZMDM", 0);

  // Path parameters are set to sensible values by
  // default, but this is how you can configure them:
  APRS_setPath1("WIDE1", 1);
  APRS_setPath2("WIDE2", 2);

  // You can define preamble and tail like this:
  // APRS_setPreamble(350);
  // APRS_setTail(50);

  // You can use the normal or alternate symbol table:
  // APRS_useAlternateSymbolTable(false);

  // And set what symbol you want to use:
  APRS_setSymbol('O');

  // We can print out all the settings
  //APRS_printSettings();
  //Serial.print(F("Free RAM:     ")); Serial.println(freeMemory());
}

void locationUpdateExample() {

  // We can optionally set power/height/gain/directivity
  // information. These functions accept ranges
  // from 0 to 10, directivity 0 to 9.
  // See this site for a calculator:
  // http://www.aprsfl.net/phgr.php
  // LibAPRS will only add PHG info if all four variables
  // are defined!
  //APRS_setPower(2);
  //APRS_setHeight(4);
  //APRS_setGain(7);
  //APRS_setDirectivity(0);

  // We'll define a comment string


  // And send the update
  APRS_setLat(latitud);
  Serial.println(latitud);
  APRS_setLon(longitud);
  Serial.println(longitud);
  APRS_sendLoc(comment, strlen(comment));

}

//Function to send standard message
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

//Function to read incomming message
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

void loop() {

  delay(10000);

  //locationUpdateExample();
  //Loop only wait until Master sends information

  delay(10);

}

//Function to read incoming packet from Master device
void receiveEvent(int by){
  i=0;
  while(9<Wire.available()){
    latitud[i] = Wire.read(); // receive a byte as character
    i++;
  }
  i=0;
  while(Wire.available()){
    longitud[i] = Wire.read(); // receive a byte as character
    i++;
  }
  locationUpdateExample();
}
