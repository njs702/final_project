#include <dht.h>
#include <SPI.h>
#include <mcp_can.h>
#include <Wire.h>
#include <SoftwareSerial.h> 

#define __MEGA2560__ 1                   //0: Uno, 1: Mega 2560

#if __MEGA2560__
#define BTSerial Serial3
#else
int blueTx=2;   
int blueRx=3;   
SoftwareSerial mySerial(blueTx, blueRx); 
#endif

#define BAUDRATE 115200                 //select 9600 or 115200 w.r.t module

/* ============== PIN NUMBER SETTINGS ============== */
static const int CAN_PIN = 53; // CAN 통신용 핀(CS, MEGA:53 & UNO:10)
static const int GYRO_PIN = 14; // 자이로 센서 값 핀
static const int DHT_11_PIN = 3; // 온,습도 센서 값 핀
static const int INTERRUPT_PIN = 2; // 인터럽트 처리 핀
static constexpr int ECHO_PIN = 8; // 초음파 센서 에코 핀
static constexpr int TRIG_PIN = 9; // 초음파 센서 트리거 핀

static const int RED_LED1_PIN = 35;
static const int LIGHT_BUTTON_PIN = 22;

static const int TX_PIN = 18;
static const int RX_PIN = 19;
/* ================================================= */



/* ============== CAN ID SETTINGS ============== */
unsigned long temp_humid_can_id = 0x11;
unsigned long gyro_data_id_front = 0x12;
unsigned long gyro_data_id_back = 0x13;
unsigned long ultra_data_id = 0x14;

unsigned long light_on_id = 0x15;
unsigned long light_off_id = 0x16;
/* ============================================= */



/* ============== GLOBAL LIBRARY VARIABLES ============== */
dht DHT; 
MCP_CAN CAN(CAN_PIN);
//SoftwareSerial BTSerial(TX_PIN, RX_PIN);
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

union distance_union{
    float first;
    unsigned char second[8];
};

/* ========================================================== */


/* ============== GLOBAL classes ============== */


/* ============================================== */


/* ============== GLOBAL variables ============== */
static temp_humid_data temp_humid;
static Vector7i data;
static const int MPU_addr = 0x68;
static char bluetooth_data = '0';

static boolean light_flag = false;

//좌우 모터 속도 조절, 설정 가능 최대 속도 : 255
int L_MotorSpeed = 100; // 왼쪽 모터 속도
int R_MotorSpeed = 100; // 오른쪽 모터 속도

int R_Motor = 0; // 오른쪽 모터 HIGH & LOW 판별 변수
int L_Motor = 0; // 왼쪽 모터 HIGH & LOW 판별 변수
int mode = 0; // 블루투스 데이터에 따른 모드 설정 변수
/* ============================================== */



/* ============== GLOBAL functions ============== */
void read_temp_humid(){
    DHT.read11(DHT_11_PIN);
    temp_humid.humid = DHT.humidity;
    temp_humid.temp = DHT.temperature;
    delay(5);
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

void initUltrasonic () {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

float readUltrasonic () {
    float return_time;
    float time_took;
    
    digitalWrite(TRIG_PIN, HIGH);
    
    delay(5);
    
    digitalWrite(TRIG_PIN, LOW);
    
    return_time = pulseIn(ECHO_PIN, HIGH);
    time_took   = 340.0f * return_time / 10000.0f / 2.0f;
    
    return time_took;
}

void sendUltraData(){
    distance_union du;
    unsigned char data[8];

    du.first = readUltrasonic();
    for(int i=0;i<8;i++){
        data[i] = du.second[i];
    }
    
    CAN.sendMsgBuf(ultra_data_id,0,8,data);
    delay(10);
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

void initButton(){
    pinMode(LIGHT_BUTTON_PIN,INPUT);
}

void turn_on_light(){
    // 버튼 눌리면 CAN BUS로 불 켜라는 신호 전달
}

void turn_off_light(){
    // 버튼 눌리면 CAN BUS로 불 끄라는 신호 전달
}

void send_light_info(){
    
}
/* ============================================== */

void setup()
{   
    initButton();
    initGyro();
    initUltrasonic();
	Serial.begin(BAUDRATE);
    /* BTSerial.begin(BAUDRATE);
    Serial.println("BTserial init Success");   */
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
    sendUltraData();
    send_gyro_data();
    send_temp_humid();
}
