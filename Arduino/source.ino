#include <dht.h>
#include <SPI.h>
#include <mcp_can.h>
#include <Wire.h>

/* ============== PIN NUMBER SETTINGS ============== */
static const int CAN_PIN = 10;
static const int GYRO_PIN = 14;
static const int DHT_11_PIN = 3;
static const int INTERRUPT_PIN = 2;
/* ================================================= */



/* ============== CAN ID SETTINGS ============== */
unsigned long temp_humid_can_id = 0x11;
unsigned long gyro_data_id_front = 0x12;
unsigned long gyro_data_id_back = 0x13;
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

struct Vector7i {
    uint16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
};

struct Vector7f {
    float AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
};

union temp_humid_union{
    temp_humid_data first;
    unsigned char second[8];
};

union vector7i_union {
    Vector7i first;
    unsigned char second[16];
};

union vector7f_union {
    Vector7f first;
    unsigned char second[16];
};

/* ========================================================== */


/* ============== GLOBAL classes ============== */

/* ============================================== */


/* ============== GLOBAL variables ============== */
static temp_humid_data temp_humid;
static Vector7i data;
static const int MPU_addr = 0x68;
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
    delay(10);


} // 온,습도 데이터 보내기

void send_gyro_data(){
    vector7i_union v7u;

    v7u.first = data;
    unsigned char front[8];
    unsigned char back[8];

    for(int i=0;i<8;i++){
        front[i] = v7u.second[i];
    }
    for(int i=0;i<8;i++){
        back[i] = v7u.second[i+8];
    }
    
    CAN.sendMsgBuf(gyro_data_id_front,0,8,front);
    delay(10);

    CAN.sendMsgBuf(gyro_data_id_back,0,8,back);
    delay(10);

} // 자이로 센서값 데이터 보내기

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

void initGyro () {
    Wire.begin();
    Wire.beginTransmission(MPU_addr); 
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);
}

void getData (Vector7i* data) {
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 14, true);

    data->AcX = Wire.read() << 8 | Wire.read();
    data->AcY = Wire.read() << 8 | Wire.read();
    data->AcZ = Wire.read() << 8 | Wire.read();
    data->Tmp = Wire.read() << 8 | Wire.read();
    data->GyX = Wire.read() << 8 | Wire.read();
    data->GyY = Wire.read() << 8 | Wire.read();
    data->GyZ = Wire.read() << 8 | Wire.read();
}
/* ============================================== */


void setup()
{   
    initGyro();
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
    getData(&data);
    Serial.println(data.AcX);
    Serial.println(data.AcY);
    send_temp_humid();
    send_gyro_data();
}
