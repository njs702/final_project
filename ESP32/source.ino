//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Victor Tchistiak - 2019
//
//This example demostrates master mode bluetooth connection and pin 
//it creates a bridge between Serial and Classical Bluetooth (SPP)
//this is an extention of the SerialToSerialBT example by Evandro Copercini - 2018
//

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

String MACadd = "AA:BB:CC:11:22:33";
uint8_t address[6]  = {0x98, 0xDA, 0x40, 0x00, 0xF4, 0x3B};
//uint8_t address[6]  = {0x00, 0x1D, 0xA5, 0x02, 0xC3, 0x22};
String name = "ZS-040test";
char *pin = "1234"; //<- standard pin would be provided by default
bool connected;

void setup() {
  Serial.begin(115200);
  SerialBT.setPin(pin); 
  SerialBT.begin("ESP32test", true); 
  SerialBT.setPin(pin);
  Serial.println("The device started in master mode, make sure remote BT device is on!");
  
  // connect(address) is fast (upto 10 secs max), connect(name) is slow (upto 30 secs max) as it needs
  // to resolve name to address first, but it allows to connect to different devices with the same name.
  // Set CoreDebugLevel to Info to view devices bluetooth address and device names
  //connected = SerialBT.connect(name);
  connected = SerialBT.connect(address);
  
  if(connected) {
    Serial.println("Connected Succesfully!");
  } else {
    while(!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app."); 
    }
  }
  // disconnect() may take upto 10 secs max
  /*
  if (SerialBT.disconnect()) {
    Serial.println("Disconnected Succesfully!");
  }
  // this would reconnect to the name(will use address, if resolved) or address used with connect(name/address).
  */
  SerialBT.connect();
}

void loop() {
  if(SerialBT.available())            //송신
  {
    Serial.println(SerialBT.read());  
  }

  if(Serial.available()){
    SerialBT.write(Serial.read());
    //SerialBT.write('1');
  }
  
  delay(20);
}