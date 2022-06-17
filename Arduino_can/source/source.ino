#include <Wire.h>
#include <SoftwareSerial.h> 

#define __MEGA2560__ 0                   //0: Uno, 1: Mega 2560

#if __MEGA2560__
#define BTSerial Serial3
#else
int blueTx=2;   
int blueRx=3;   
SoftwareSerial mySerial(blueTx, blueRx); 
#endif

#define BAUDRATE 115200                 //select 9600 or 115200 w.r.t module

/* ============== PIN NUMBER SETTINGS ============== */
static const int RightMotor_E_pin = 4; // 오른쪽 모터의 Enable & PWM
static const int LeftMotor_E_pin = 5;  // 왼쪽 모터의 Enable & PWM
static const int RightMotor_1_pin = 6; // 오른쪽 모터 제어선 IN1
static const int RightMotor_2_pin = 7; // 오른쪽 모터 제어선 IN2
static const int LeftMotor_3_pin = 11; // 왼쪽 모터 제어선 IN3
static const int LeftMotor_4_pin = 12; // 왼쪽 모터 제어선 IN4

/* static const int RightMotor_E_pin = 0; // 오른쪽 모터의 Enable & PWM
static const int LeftMotor_E_pin = 1;  // 왼쪽 모터의 Enable & PWM
static const int RightMotor_1_pin = 4; // 오른쪽 모터 제어선 IN1
static const int RightMotor_2_pin = 5; // 오른쪽 모터 제어선 IN2
static const int LeftMotor_3_pin = 6; // 왼쪽 모터 제어선 IN3
static const int LeftMotor_4_pin = 7; // 왼쪽 모터 제어선 IN4 */

static const int led_yellow_pin = 8;
static const int led_red_pin = 9;
/* ================================================= */

/* ============== GLOBAL variables ============== */
static char bluetooth_data = '0';

//좌우 모터 속도 조절, 설정 가능 최대 속도 : 255
int L_MotorSpeed = 200; // 왼쪽 모터 속도
int R_MotorSpeed = 180; // 오른쪽 모터 속도

int R_Motor = 0; // 오른쪽 모터 HIGH & LOW 판별 변수
int L_Motor = 0; // 왼쪽 모터 HIGH & LOW 판별 변수
int mode = 0; // 블루투스 데이터에 따른 모드 설정 변수
/* ============================================== */

/* ============== GLOBAL functions ============== */
void initMotor(){
    pinMode(RightMotor_E_pin, OUTPUT);// 출력모드로 설정
    pinMode(RightMotor_1_pin, OUTPUT);
    pinMode(RightMotor_2_pin, OUTPUT);
    pinMode(LeftMotor_3_pin, OUTPUT);
    pinMode(LeftMotor_4_pin, OUTPUT);
    pinMode(LeftMotor_E_pin, OUTPUT);
    Serial.println("Motor init Success"); 
}

void initLed(){
    pinMode(led_yellow_pin,OUTPUT);
    pinMode(led_red_pin,OUTPUT);
}

void control_smartCar(char data){
    digitalWrite(led_red_pin,LOW);
    digitalWrite(led_yellow_pin,LOW);
    switch (data)
    {
    case 'g':
        R_Motor = HIGH; L_Motor = HIGH; mode = 0;
        digitalWrite(led_yellow_pin,HIGH);
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
        digitalWrite(led_yellow_pin,LOW);
        break; // 정지

    case 'q':
        mode = 4;
        break; // 제자리 좌회전
    
    case 'W':
        mode = 5;
        break; // 제자리 우회전
    
    case 'k':
        R_Motor = HIGH; L_Motor = HIGH; mode = 3;
        digitalWrite(led_red_pin,HIGH);
        break;

    default:
        //mode = 6;
        break;
    }
}

void motor_role(int R_motor, int L_motor){
    digitalWrite(RightMotor_1_pin, R_motor);
    digitalWrite(RightMotor_2_pin, !R_motor);
    digitalWrite(LeftMotor_3_pin, L_motor);
    digitalWrite(LeftMotor_4_pin, !L_motor);

    analogWrite(RightMotor_E_pin, R_MotorSpeed);// 우측 모터 속도값
    analogWrite(LeftMotor_E_pin, L_MotorSpeed);// 좌측 모터 속도값  
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

void temp_test(int R_motor, int L_motor){
    digitalWrite(RightMotor_1_pin, R_motor);
    digitalWrite(RightMotor_2_pin, !R_motor);
    digitalWrite(LeftMotor_3_pin, L_motor);
    digitalWrite(LeftMotor_4_pin, !L_motor);

    for(int i = R_MotorSpeed, j = L_MotorSpeed; i < 60, j < 60; i -= 10, j -= 10){
        analogWrite(RightMotor_E_pin,i);
        analogWrite(LeftMotor_E_pin,j);
    }
}

void joystick_read(){
    
    if(mySerial.available()){
        bluetooth_data = mySerial.read();
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
        /* case 3:
            analogWrite(RightMotor_E_pin, 0);
            analogWrite(LeftMotor_E_pin, 0);
            break; */
        case 4:
            left_rotation(R_Motor, L_Motor);
            break;
        case 5:
            right_rotation(R_Motor, L_Motor);
            break;
        case 6:
            temp_test(R_Motor,L_Motor);
            break;    
        default:
            analogWrite(RightMotor_E_pin, 0);
            analogWrite(LeftMotor_E_pin, 0);
            //motor_role(R_Motor, L_Motor);
            break;
        }

    }
    
}
/* ============================================== */

void setup(){
    Serial.begin(BAUDRATE);
    initMotor();
    initLed();
    mySerial.begin(BAUDRATE);
    Serial.println("BTserial init Success");  
}

void loop(){
    joystick_read();
}