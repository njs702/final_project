
#include <SoftwareSerial.h>                // Serial 통신을 하기 위해 선언

SoftwareSerial BTSerial(2, 3);             // HC-06모듈 RX=3, TX=2
void setup()
{
    Serial.begin(115200);                          // 시리얼 통신 선언 (보드레이트 9600)
    BTSerial.begin(115200);                     // HC-06 모듈 통신 선언 (보드레이트 9600)
}
void loop() 

{

    if(BTSerial.available())                     // BTSerial에 입력이 되면
    Serial.write(BTSerial.read());           // BTSerial에 입력된 값을 시리얼 모니터에 출력


    if(Serial.available())                          // 시리얼 모니터에 입력이 되면

    BTSerial.write(Serial.read());           // 그 값을 BTSerial에 출력

}