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
static const int DHT_11_PIN = 13; // 온,습도 센서 값 핀
static const int INTERRUPT_PIN = 2; // 인터럽트 처리 핀
static constexpr int ECHO_PIN = 8; // 초음파 센서 에코 핀
static constexpr int TRIG_PIN = 9; // 초음파 센서 트리거 핀

static const int RightMotor_E_pin = 4; // 오른쪽 모터의 Enable & PWM
static const int LeftMotor_E_pin = 5;  // 왼쪽 모터의 Enable & PWM
static const int RightMotor_1_pin = 6; // 오른쪽 모터 제어선 IN1
static const int RightMotor_2_pin = 7; // 오른쪽 모터 제어선 IN2
static const int LeftMotor_3_pin = 11; // 왼쪽 모터 제어선 IN3
static const int LeftMotor_4_pin = 12; // 왼쪽 모터 제어선 IN4

static const int TX_PIN = 18;
static const int RX_PIN = 19;
/* ================================================= */



/* ============== CAN ID SETTINGS ============== */
unsigned long temp_humid_can_id = 0x11;
unsigned long gyro_data_id_front = 0x12;
unsigned long gyro_data_id_back = 0x13;
unsigned long ultra_data_id = 0x14;
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

void control_smartCar(char data){
    switch (data)
    {
    case 'g':
        R_Motor = HIGH; L_Motor = HIGH; mode = 0;
        break; // 전진
    
    case 'r':
        mode = 1;
        break; // 우회전
    
    case 'l':
        mode = 2;
        break; // 좌회전

    case 'b':
        R_Motor = LOW; L_Motor = LOW; mode = 0;
        break; // 후진

    case 's':
        R_Motor = HIGH; L_Motor = HIGH; mode = 3;
        break; // 정지

    case 'q':
        mode = 4;
        break; // 제자리 좌회전
    
    case 'W':
        mode = 5;
        break; // 제자리 우회전

    default:
        break;
    }
}

void motor_role(int R_motor, int L_motor){
    digitalWrite(RightMotor_1_pin, R_motor);
    digitalWrite(RightMotor_2_pin, !R_motor);
    digitalWrite(LeftMotor_3_pin, L_motor);
    digitalWrite(LeftMotor_4_pin, !L_motor);
    
    analogWrite(RightMotor_E_pin, R_MotorSpeed);                                           // 우측 모터 속도값
    analogWrite(LeftMotor_E_pin, L_MotorSpeed);                                         // 좌측 모터 속도값  
}

void Right_role(int R_motor, int L_motor){
    digitalWrite(RightMotor_1_pin, R_motor);
    digitalWrite(RightMotor_2_pin, !R_motor);
    digitalWrite(LeftMotor_3_pin, L_motor);
    digitalWrite(LeftMotor_4_pin, !L_motor);
    
    analogWrite(RightMotor_E_pin, max(R_MotorSpeed*0.4,90));// 우측 모터 속도값
    analogWrite(LeftMotor_E_pin, 255);// 좌측 모터 속도값
}

void Left_role(int R_motor, int L_motor){
    digitalWrite(RightMotor_1_pin, R_motor);
    digitalWrite(RightMotor_2_pin, !R_motor);
    digitalWrite(LeftMotor_3_pin, L_motor);
    digitalWrite(LeftMotor_4_pin, !L_motor);
    
    analogWrite(RightMotor_E_pin, 255);                        // 우측 모터 속도값
    analogWrite(LeftMotor_E_pin, max(L_MotorSpeed*0.4,90));                               // 좌측 모터 속도값   
}

void left_rotation(int R_motor, int L_motor){
    digitalWrite(RightMotor_1_pin, HIGH);
    digitalWrite(RightMotor_2_pin, LOW);
    digitalWrite(LeftMotor_3_pin, LOW);
    digitalWrite(LeftMotor_4_pin, HIGH);
    
    analogWrite(RightMotor_E_pin, R_MotorSpeed);                         // 우측 모터 속도값
    analogWrite(LeftMotor_E_pin, L_MotorSpeed);                               // 좌측 모터 속도값
}

void right_rotation(int R_motor, int L_motor){
    digitalWrite(RightMotor_1_pin, LOW);
    digitalWrite(RightMotor_2_pin, HIGH);
    digitalWrite(LeftMotor_3_pin, HIGH);
    digitalWrite(LeftMotor_4_pin, LOW);
    
    analogWrite(RightMotor_E_pin, R_MotorSpeed);                         // 우측 모터 속도값
    analogWrite(LeftMotor_E_pin, L_MotorSpeed);                               // 좌측 모터 속도값
}

void joystick_read(){
    
    if(BTSerial.available()){
        bluetooth_data = BTSerial.read();
        Serial.println(bluetooth_data);
        control_smartCar(bluetooth_data);
        
        // mode에 따른 자동차의 동작 제어 분기문
        switch (mode)
        {
        case 0:
            motor_role(R_Motor, L_Motor);
            break;
        case 1:
            Right_role(R_Motor, L_Motor);
            break;
        case 2:
            Left_role(R_Motor, L_Motor);
            break;
        case 4:
            left_rotation(R_Motor, L_Motor);
            break;
        case 5:
            right_rotation(R_Motor, L_Motor);
            break;    
        default:
            analogWrite(RightMotor_E_pin, 0);
            analogWrite(LeftMotor_E_pin, 0);
            break;
        }

    }
    
}

void initMotor(){
    pinMode(RightMotor_E_pin, OUTPUT);// 출력모드로 설정
    pinMode(RightMotor_1_pin, OUTPUT);
    pinMode(RightMotor_2_pin, OUTPUT);
    pinMode(LeftMotor_3_pin, OUTPUT);
    pinMode(LeftMotor_4_pin, OUTPUT);
    pinMode(LeftMotor_E_pin, OUTPUT);
}
/* ============================================== */

void setup()
{   
    initMotor();
    initGyro();
    initUltrasonic();
	Serial.begin(BAUDRATE);
    BTSerial.begin(BAUDRATE);
    Serial.println("BTserial init Success");  
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
    //joystick_read();
}
