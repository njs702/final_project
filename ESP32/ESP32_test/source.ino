#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

String MACadd = "AA:BB:CC:11:22:33";
uint8_t address[6]  = {0x98, 0xDA, 0x40, 0x00, 0xF4, 0x3B};
String name = "ZS-040test";
char *pin = "1234"; 
bool connected;

void initSerialBT(){
  SerialBT.setPin(pin); 
  SerialBT.begin("ESP32test", true); 
  SerialBT.setPin(pin);
}

void connectedtest(){
   connected = SerialBT.connect(address);
   if(connected) {
    Serial.println("Connected Succesfully!");
  } else {
    while(!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app."); 
    }
  }
  SerialBT.connect();
}

void setup() {
  Serial.begin(115200);
  initSerialBT();
  Serial.println("The device started in master mode, make sure remote BT device is on!");
  connectedtest();
}

void loop() {
  if(SerialBT.available())            //송신
  {
    Serial.println(SerialBT.read());  
  }
  if(Serial.available()){
    SerialBT.write(Serial.read());
  }
  delay(20);
}