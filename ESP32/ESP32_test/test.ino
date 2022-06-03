
int joystick_x = 36;   
int joystick_y = 39;
int joystick_z = 22;    //조이스틱 X,Y,Z축을 저장

void setup()
{
 pinMode(joystick_z,INPUT_PULLUP);    // Z를 입력모드로
 Serial.begin(115200);               // 시리얼 통신을, 9600속도로 받습니다. (숫자 조정은 자유)
}

void loop()
{
 int x = analogRead(joystick_x);      // 조이스틱 X축의 아날로그 신호를 x에 저장
 int y = analogRead(joystick_y);      // 조이스틱 Y축의 아날로그 신호를 y에 저장
 int z = digitalRead(joystick_z);     // 조이스틱 Z축의 아날로그 신호를 z에 저장 
  Serial.print("X: ");                   
  Serial.print(x);                          //시리얼 모니터에 'X: x값'을 표기
  Serial.print("  Y: ");
  Serial.print(y);                          //시리얼 모니터에 '  Y: y값'을 표기
  Serial.print("  Z: ");
  Serial.println(z);                         //시리얼 모니터에 '  Z: z값'을 표기하고 줄넘김
delay(100);                                  // 100만큼 지연
} 
/*
const int AXIS_X = 36;
const int AXIS_Y = 39;
const int SW_P = 22; 
 
void setup() {
  Serial.begin(115200);
  pinMode(SW_P,INPUT_PULLUP);
}
 void loop() {
  //X축 방향값
  if(analogRead(AXIS_X)<=300){
    Serial.println('a');
  }
  else if(analogRead(AXIS_X)>=700){
    Serial.println('d');   
  }
  //Y축 방향값
  if(analogRead(AXIS_Y)<=300){
    Serial.println('w');      
  }
  else if(analogRead(AXIS_Y)>=700){
    Serial.println('s');
  }
  delay(20);
}
*/