#include <SoftwareSerial.h>
SoftwareSerial BTSerial(2, 3);  
void setup() {
  Serial.begin(115200);
  BTSerial.begin(115200);
}

void loop() { 
  if (BTSerial.available()){ //안드로이드 쪽 명령
    char msg = BTSerial.read();
    switch(msg){
      case 'a':
        digitalWrite(9,HIGH);
        digitalWrite(10,LOW);
        break;
      case 'b':
        digitalWrite(9,LOW);
        digitalWrite(10,HIGH);
        break;
    }
    Serial.write(msg);
   
  }
  if (Serial.available()){
    char msg01 = Serial.read();
    BTSerial.write(msg01);
  }
}