#include <SPI.h>
#include <mcp_can.h>

int led_pin = 9;
static boolean light_flag = false;
MCP_CAN CAN(10);

void CAN_INT(){
    unsigned char len = 0;
    unsigned char buf[8];

    CAN.readMsgBuf(&len,buf); // CAN 데이터 가져오기
    unsigned long canId = CAN.getCanId(); // CAN ID 얻기

    switch (canId)
    {
    case 0x15:
        light_flag = true;
        break;
    case 0x16:
        light_flag = false;
        break;
    case 0x17:
        light_flag = true;
        break;
    case 0x18:
        light_flag = false;
        break;
    default:
        break;
    }
}

void setup(){
    pinMode(led_pin,OUTPUT);
    Serial.begin(115200);
    while(CAN_OK != CAN.begin(CAN_500KBPS,MCP_8MHz)){
        Serial.println("CAN BUS init failed!");
        delay(100);
    }
    Serial.println("CAN BUS init Success!");
    // 2번 핀 인터럽트 설정(falling 때), 인터럽트 들어오면 CAN_int 함수 실행
    attachInterrupt(digitalPinToInterrupt(2),CAN_INT,FALLING);
}

void loop(){
    if(light_flag == true){
        digitalWrite(led_pin,HIGH);
    }
    if(light_flag == false){
        digitalWrite(led_pin,LOW);
    }
    Serial.println(light_flag);
}