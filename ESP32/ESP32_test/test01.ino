int val_0 =0;
int val_1 =0;
int AXIS_X=36;
int AXIS_Y=39;

void setup() {
  Serial.begin(115200);
}

void loop() {
  val_0 = analogRead(AXIS_X);
  val_1 = analogRead(AXIS_Y);
    
  Serial.print("VRx : ");
  Serial.print(val_0);
  Serial.print("    ");
  Serial.print("VRy : ");
  Serial.println(val_1);
  
  if(analogRead(AXIS_X)==0&&analogRead(AXIS_Y)>1900){
      Serial.println("1");//상
  }
  if(analogRead(AXIS_X)>1900&&analogRead(AXIS_Y)==0){
      Serial.println("2");//우
  }
  if(analogRead(AXIS_X)>1900&&analogRead(AXIS_Y)>4000){
      Serial.println("3");//좌
  }
  if(analogRead(AXIS_X)>4000&&analogRead(AXIS_Y)<1900){
      Serial.println("4");//하
  }
  
  delay(200);
}
