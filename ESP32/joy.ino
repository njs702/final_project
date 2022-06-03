#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

int val_0 =0;
int val_1 =0;
int AXIS_X=36;
int AXIS_Y=39;

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
  //Serial.println("The device started in master mode, make sure remote BT device is on!");
  connectedtest();
}

void loop() {
  val_0 = analogRead(AXIS_X);
  val_1 = analogRead(AXIS_Y);
  char msg = '0';
 
  if(analogRead(AXIS_X)==0&&analogRead(AXIS_Y)>1900){
      msg='1';//상
  }
  if(analogRead(AXIS_X)>1900&&analogRead(AXIS_Y)==0){
      msg='2';//우
  }
  if(analogRead(AXIS_X)>1900&&analogRead(AXIS_Y)>4000){
      msg='3';//좌
  }
  if(analogRead(AXIS_X)>4000&&analogRead(AXIS_Y)<1900){
      msg='4';//하
  }
  Serial.println(msg);
  SerialBT.write(msg);
  
  delay(200);
}
