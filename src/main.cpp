// Include the libraries we need
#include <WIFIConnector_MKR1010.h>
#include "MQTTConnector.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include<string.h>


// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9
void printAddress(DeviceAddress deviceAddress); 
String adresse; 
unsigned long TempsActuel, TempsAvant, DelayRequest = 10000; 

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress insideThermometer, outsideThermometer;


void setup(void)
{  

  wifiConnect(); // Branchement au WIFI 
  MQTTConnect(); // Branchement au broker MQTT 

  appendPayloadString("Mac Adress ", MacAdress);   //Ajout de la donnee Temperature au message MQTT
  sendPayloadString(); //Envoie du message via le protocole MQTT

  // start serial port
  Serial.begin(9600);

  // Start up the library
  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // Search for devices on the bus and assign based on an index. Ideally,
  // you would do this to initially discover addresses on the bus and then
  // use those addresses and manually assign them (see above) once you know
  // the devices on your bus (and assuming they don't change).
  //
  // method 1: by index

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(outsideThermometer, 1)) Serial.println("Unable to find address for Device 1");

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(outsideThermometer);
  Serial.println();

  // set the resolution to 9 bit per device
  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);
 
  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC);
  Serial.println();

  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(outsideThermometer), DEC);
  Serial.println();

}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{ 
  adresse ="";
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) adresse += '0';
    adresse += String(deviceAddress[i], HEX);
  }

  Serial.print(adresse);
}


// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();
}

// main function to print information about a device
void SendData(DeviceAddress deviceAddress)
{
  
  printAddress(deviceAddress);
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print(" Temperature : ");
  Serial.println(tempC);
  adresse.toUpperCase();
  appendPayload(adresse, tempC); //Ajout de la donnee Temperature au message MQTT
  sendPayload();    //Envoie du message via le protocole MQTT
}

/*
   Main function, calls the temperatures in a loop.
*/
void loop(void)
{
   
  TempsActuel = millis(); 

  if (TempsActuel - TempsAvant > DelayRequest)
  {
    
      // call sensors.requestTemperatures() to issue a global temperature
      // request to all devices on the bus
      Serial.print("Requesting temperatures...");
      sensors.requestTemperatures();
      Serial.println("DONE");

      // print the device information
      SendData(insideThermometer);
      SendData(outsideThermometer);
      Serial.println();
      TempsAvant = TempsActuel;

  }
  
}