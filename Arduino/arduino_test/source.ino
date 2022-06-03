#include <dht.h>
#include <SPI.h>
#include <mcp_can.h>

/* ============== PIN NUMBER SETTINGS ============== */
static const int CAN_PIN = 10;
static const int DHT_11_PIN = 3;
static const int INTERRUPT_PIN = 2;
/* ================================================= */



/* ============== CAN ID SETTINGS ============== */
unsigned long temp_humid_can_id = 0x11;
/* ============================================= */



/* ============== GLOBAL LIBRARY VARIABLES ============== */
dht DHT; 
MCP_CAN CAN(CAN_PIN);
/* ====================================================== */



/* ============== GLOBAL custom struct & union ============== */
typedef struct{
    float humid; 
    float temp;
}temp_humid_data;

union temp_humid_union{
    temp_humid_data first;
    unsigned char second[8];
};
/* ========================================================== */



/* ============== GLOBAL variables ============== */
static temp_humid_data temp_humid;
/* ============================================== */



/* ============== GLOBAL functions ============== */
void read_temp_humid(){
    DHT.read11(DHT_11_PIN);
    temp_humid.humid = DHT.humidity;
    temp_humid.temp = DHT.temperature;
} // 온,습도 데이터 읽어오기

void send_temp_humid(){
    read_temp_humid();

    temp_humid_union thu;
    thu.first = temp_humid;
    unsigned char stmp[8];

    for(int i=0;i<8;i++){
        stmp[i] = thu.second[i];
    }
    CAN.sendMsgBuf(temp_humid_can_id,0,8,stmp);
    delay(100);
} // 온,습도 데이터 보내기

void CAN_INT(){
    unsigned char len = 0;
    unsigned char buf[8];

    CAN.readMsgBuf(&len,buf); // get CAN data
    unsigned long canId = CAN.getCanId(); // get CAN id
    
    /* Process according to ID value */
    switch (canId)
    {
    case 0x12:
        /* code */
        CAN.readMsgBuf(&len,buf); // CAN 데이터 가져오기
        Serial.print("\nData from ID:0x");
        Serial.println(canId,HEX);
        for(int i=0;i<len;i++){
            Serial.print(buf[i]);
            Serial.print("\t");
        }
        break;
    
    default:
        break;
    }

} // Interrupt 발생 시 처리
/* ============================================== */

void setup()
{
	Serial.begin(115200);
    while(CAN_OK != CAN.begin(CAN_500KBPS,MCP_8MHz)){
        Serial.println("CAN BUS init Failed");
        delay(100);
    }
    Serial.println("CAN BUS init Success");

    // Run can_int function when interrupt occurs
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN),CAN_INT,FALLING);

}

void loop()
{
	send_temp_humid();
}
