#include <PS2X_lib.h>

#include <Wire.h>

#include <Servo.h>
//เปลี่ยนไปใช้ arduino mega แล้วจร้าาา
int buildInLED = 13;
#define PS2_DAT 12
#define PS2_CMD 11
#define PS2_CLK 10
#define PS2_ATT 9
#define stepPin 26
#define dirPin 27
#define shooterPin 8
#define Left_I2C_ADDRESS 0x50
#define Right_I2C_ADDRESS 0x57
#define lift_I2C_ADDRESS 0x51
#define liftMotorPwm 1
#define liftMotorIn1 2
#define liftMotorIn2 3
Servo myservo;

// motor1 => 0x85 (front), motor2 => 0x86 (back)

bool liftBreak = 0;
unsigned long currentTimeLED = millis();
unsigned long currentTimeStepper = micros();
unsigned long currentforward = millis();
unsigned long currentbackward = millis();
int selectOn = 150;
float speed1=0;
float speed2=0;
int RollerSpeed = 0;
float true_RY = 0;
float true_RX = 0;
bool check_RY = 0;
bool check_RX = 0;
#define pressures false
#define rumble false

int error = 0;
byte type = 0;
byte vibrate = 0;

int prevDir = 3;

void controlMotor(int Speed, int *PWM, int *Dir) {
  if (Speed > 0) {  //กรณี Speed มีค่าเป็นบวก => ตามเข็ม
    *PWM = Speed;
    *Dir = 1;
  } else if (Speed < 0) {  //กรณี Speed มีค่าเป็นลบ => ทวนเข็ม
    *PWM = -Speed;
    *Dir = 2;
  } else if (Speed == 0) {  // กรณี Speed มีค่าเป็นศูนย์ => หยุดหมุน
    *PWM = 0;
    *Dir = 3;
  } else if (Speed == -1) {
    PWM = 0;
    if (prevDir == 1) *Dir = 2;
    if (prevDir == 2) *Dir = 1;
  }
  if (*PWM > 255) *PWM = 255;  //จำกัดให้ PWM Speed มีค่าไม่เกิน 255
}

void frontLeftMotor(int Speed) {
  int PWM;
  int Dir;
  controlMotor(Speed, &PWM, &Dir);
  prevDir = Dir;
  Wire.beginTransmission(Left_I2C_ADDRESS);  //เริ่มส่งข้อมูลไขยัง Address ที่กำหนดไว้ใน MOIOR_ I2C_ADDRESS
  Wire.write(0x85);
  Wire.write(Dir);  //ส่งทิศทางการหมุน มีค่าระหว่าง 1-3
  Wire.write(PWM);  //ส่งความเร็วมอเตอร์ มีค่าระหว่าง 0-255
  Wire.endTransmission();
}
void backLeftMotor(int Speed) {
  int PWM;
  int Dir;
  controlMotor(Speed, &PWM, &Dir);
  prevDir = Dir;
  Wire.beginTransmission(Left_I2C_ADDRESS);  //เริ่มส่งข้อมูลไขยัง Address ที่กำหนดไว้ใน MOIOR_ I2C_ADDRESS
  Wire.write(0x86);
  Wire.write(Dir);  //ส่งทิศทางหมุนสำหรับมอเตอร์ตัวที่ 1 ซึ่งมีค่าเท่ากับ 1-3
  Wire.write(PWM);  //ส่ง Command : Motor 1 control
  Wire.endTransmission();
}
void frontRightMotor(int Speed) {
  int PWM;
  int Dir;

  controlMotor(Speed, &PWM, &Dir);
  prevDir = Dir;
  Wire.beginTransmission(Right_I2C_ADDRESS);  //เริ่มส่งข้อมูลไขยัง Address ที่กำหนดไว้ใน MOIOR_ I2C_ADDRESS
  Wire.write(0x85);
  Wire.write(Dir);  //ส่งทิศทางหมุนสำหรับมอเตอร์ตัวที่ 1 ซึ่งมีค่าเท่ากับ 1-3
  Wire.write(PWM);  //ส่ง Command : Motor 1 control
  Wire.endTransmission();
}
void backRightMotor(int Speed) {
  int PWM;
  int Dir;
  controlMotor(Speed, &PWM, &Dir);
  prevDir = Dir;
  Wire.beginTransmission(Right_I2C_ADDRESS);  //เริ่มส่งข้อมูลไขยัง Address ที่กำหนดไว้ใน MOIOR_ I2C_ADDRESS
  Wire.write(0x86);
  Wire.write(Dir);  //ส่งทิศทางหมุนสำหรับมอเตอร์ตัวที่ 1 ซึ่งมีค่าเท่ากับ 1-3
  Wire.write(PWM);  //ส่ง Command : Motor 1 control
  Wire.endTransmission();
}
PS2X ps2x;  // create PS2 Controller Class
void setup() {
  pinMode(buildInLED, OUTPUT);
  pinMode(PS2_ATT, OUTPUT);
  pinMode(PS2_CLK, OUTPUT);
  pinMode(PS2_CMD, OUTPUT);
  pinMode(PS2_DAT, OUTPUT);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  pinMode(shooterPin, OUTPUT);

  pinMode(liftMotorPwm, OUTPUT);
  pinMode(liftMotorIn1, OUTPUT);
  pinMode(liftMotorIn2, OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT, pressures, rumble);

  delay(1000);
  Serial.println("start");
}
// motion control
void frontLeft(int Speed) {
  frontLeftMotor(Speed);
}
void backLeft(int Speed) {
  backLeftMotor(Speed);
}
void frontRight(int Speed) {
  frontRightMotor(Speed);
}
void backRight(int Speed) {
  backRightMotor(Speed);
}
void moveFromJoystick(float trueX,float trueY){
  if(check_RX&&check_RY){
    if(trueY>0){
        if(trueX>0){
          if(trueY>trueX){
            frontLeftMotor(trueY);
            frontRightMotor(trueY);
            float newSpeed = map(trueY-trueX,150 ,0, 150, 95);
            backLeftMotor(newSpeed);
            backRightMotor(newSpeed);
          }else{
            frontLeftMotor(trueX);
            frontRightMotor(trueX);
            float newSpeed = map(trueX-trueY,150 ,0, 95, 20);
            backLeftMotor(newSpeed);
            backRightMotor(newSpeed);
          }
        }else{
          if(abs(trueY)>abs(trueX)){
            float newSpeed = map(abs(trueY)-abs(trueX),150 ,0, 150, 95);
            frontLeftMotor(newSpeed);
            frontRightMotor(newSpeed);
            backLeftMotor(trueY);
            backRightMotor(trueY);
          }else{
            float newSpeed = map(abs(trueY)-abs(trueX),150 ,0, 95, 20);
            frontLeftMotor(newSpeed);
            frontRightMotor(newSpeed);
            backLeftMotor(-trueX);
            backRightMotor(-trueX);
          }
        }   
      }else{
        if(trueX>0){
          if(abs(trueY)>abs(trueX)){
            frontLeftMotor(trueY);
            frontRightMotor(trueY);
            float newSpeed = map(abs(trueY)-abs(trueX),150 ,0, -150, -95);
            backLeftMotor(newSpeed);
            backRightMotor(newSpeed);
          }else{
            frontLeftMotor(trueX);
            frontRightMotor(trueX);
            float newSpeed = map(abs(trueX)-abs(trueY),150 ,0, -95, -20);
            backLeftMotor(newSpeed);
            backRightMotor(newSpeed);
          }
        }else{
          if(abs(trueY)>abs(trueX)){
            float newSpeed = map(abs(trueX)-abs(trueY),150 ,0, -150, -95);
            frontLeftMotor(newSpeed);
            frontRightMotor(newSpeed);
            backLeftMotor(trueY);
            backRightMotor(trueY);
          }else{
            float newSpeed = map(abs(trueX)-abs(trueY),150 ,0, -95, -20);
            frontLeftMotor(newSpeed);
            frontRightMotor(newSpeed);
            backLeftMotor(trueX);
            backRightMotor(trueX);
          }
          
        } 
      }
  }else if(check_RX&&!check_RY){
      if(trueX>0){
          frontLeftMotor(trueX);
          frontRightMotor(trueX);
          backLeftMotor(-trueX);
          backRightMotor(-trueX);
        }else{
          frontLeftMotor(trueX);
          frontRightMotor(trueX);
          backLeftMotor(-trueX);
          backRightMotor(-trueX);
        }   
  }else if(!check_RX&&check_RY){
      if(trueY>0){
        frontLeftMotor(trueY);
        frontRightMotor(trueY);
        backLeftMotor(trueY);
        backRightMotor(trueY);
      }else{
        frontLeftMotor(trueY);
        frontRightMotor(trueY);
        backLeftMotor(trueY);
        backRightMotor(trueY);
      }
    }
  
}
void slideRight(int Speed) {
  frontLeftMotor(Speed);
  backLeftMotor(-Speed);
  frontRightMotor(-Speed);
  backRightMotor(Speed);
}
void slideLeft(int Speed) {
  frontLeftMotor(-Speed);
  backLeftMotor(Speed);
  frontRightMotor(Speed);
  backRightMotor(-Speed);
}
void forwardLeft(int Speed) {
  frontLeftMotor(0);
  backLeftMotor(Speed);
  frontRightMotor(Speed);
  backRightMotor(0);
}
void forwardRight(int Speed) {
  frontLeftMotor(Speed);
  backLeftMotor(0);
  frontRightMotor(0);
  backRightMotor(Speed);
}
void backwardLeft(int Speed) {
  frontLeftMotor(-Speed);
  backLeftMotor(0);
  frontRightMotor(0);
  backRightMotor(-Speed);
}
void backwardRight(int Speed) {
  frontLeftMotor(0);
  backLeftMotor(-Speed);
  frontRightMotor(-Speed);
  backRightMotor(0);
}
void rotateCCW(int Speed) {
  frontLeftMotor(-Speed);
  backLeftMotor(-Speed);
  frontRightMotor(Speed);
  backRightMotor(Speed);
}
void rotateCW(int Speed) {
  frontLeftMotor(Speed);
  backLeftMotor(Speed);
  frontRightMotor(-Speed);
  backRightMotor(-Speed);
}
void Stop() {
  frontLeftMotor(0);
  backLeftMotor(0);
  frontRightMotor(0);
  backRightMotor(0);
}

void checkPS2Connection() {
  if (millis() - currentTimeLED > 100) {
    currentTimeLED = millis();
    digitalWrite(buildInLED, !digitalRead(buildInLED));  //สิ่งที่จะให้ทำเมื่อเวลาผ่านไปทุกๆ 100 ms
  }
}
void padCounter() {

  if (ps2x.ButtonPressed(PSB_PAD_UP) && ps2x.Button(PSB_L2) && ps2x.Button(PSB_L1)) {
    currentforward = millis() + 530;
  }
  if (ps2x.ButtonPressed(PSB_PAD_DOWN) && ps2x.Button(PSB_L2) && ps2x.Button(PSB_L1)) {
    currentbackward = millis() + 220;
  }
}

void padControl(int Speed) {

  //if (currentforward >= millis()) moveForward(Speed);
  //else if (currentbackward >= millis()) moveBackward(Speed);

  //else if (ps2x.Button(PSB_PAD_UP) && ps2x.Button(PSB_PAD_LEFT)) forwardLeft(Speed);
  //else if (ps2x.Button(PSB_PAD_UP) && ps2x.Button(PSB_PAD_RIGHT)) forwardRight(Speed);
  //else if (ps2x.Button(PSB_PAD_DOWN) && ps2x.Button(PSB_PAD_LEFT)) backwardLeft(Speed);
  //else if (ps2x.Button(PSB_PAD_DOWN) && ps2x.Button(PSB_PAD_RIGHT)) backwardRight(Speed);


  // Serial.println("Hi");
  check_RY = 0;
  check_RX = 0;
  true_RX=0;
  true_RY=0;
  if(ps2x.Analog(PSS_RY) >= 150 )//real center value is 128, but 140 is needed because controller is HIGHLY sensitive
  {
    true_RY = map(ps2x.Analog(PSS_RY),150 , 255, 0, -Speed);
    check_RY = 1;
  }
  else if(ps2x.Analog(PSS_RY) <= 100) //Same as above
  {
    true_RY = map(ps2x.Analog(PSS_RY),0 ,114 , Speed, 0);
    check_RY = 1;// create a set range for values and set output values respectively
  }  
  if(ps2x.Analog(PSS_RX) >= 150 )//real center value is 128, but 140 is needed because controller is HIGHLY sensitive
  {
    check_RX = 1;
    true_RX = map(ps2x.Analog(PSS_RX),150 , 255, 0 , Speed);
  }
  else if(ps2x.Analog(PSS_RX) <= 114) //Same as above
  {
    check_RX = 1;
    true_RX = map(ps2x.Analog(PSS_RX),0 ,114 , -Speed , 0);// create a set range for values and set output values respectively
  }  
  if(ps2x.Button(PSB_PAD_LEFT )){
    slideLeft(Speed);
  }else if(ps2x.Button(PSB_PAD_RIGHT )){
    slideRight(Speed);
  }else if(!check_RX&&!check_RY){
      
      frontLeftMotor(0);
      frontRightMotor(0);
      backLeftMotor(0);
      backRightMotor(0);
    
  }
  moveFromJoystick(true_RX,true_RY);
    




  
}
// Stepper motor
void stepperControl(bool dir, int Speed, int steps) {
  digitalWrite(dirPin, dir);
  int i = 0;
  while (i < (steps * 2)) {
    if (micros() - currentTimeStepper > Speed) {
      currentTimeStepper = micros();
      digitalWrite(stepPin, !digitalRead(stepPin));
      i++;
    }
  }
}

void liftControl() {
  if (ps2x.Button(PSB_TRIANGLE)) {
    analogWrite(liftMotorIn1, 80);
    analogWrite(liftMotorIn2, 0);
  } else if (ps2x.Button(PSB_CROSS)) {
    analogWrite(liftMotorIn1, 0);
    analogWrite(liftMotorIn2, 50);
  }

  else if (liftBreak) {
    analogWrite(liftMotorIn1, 255);
    analogWrite(liftMotorIn2, 255);
  } else {
    analogWrite(liftMotorIn1, 0);
    analogWrite(liftMotorIn2, 0);
  }

  if (ps2x.ButtonPressed(PSB_SELECT)) {
    if (liftBreak) liftBreak = 0;
    else liftBreak = 1;
  }
}
void lift(int Speed) {
  int PWM;
  int Dir;
  controlMotor(Speed, &PWM, &Dir);
  Wire.beginTransmission(lift_I2C_ADDRESS); //เริ่มส่งข้อมูลไขยัง Address ที่กำหนดไว้ใน MOIOR_ I2C_ADDRESS
  Wire.write(0x85);
  Wire.write(Dir); //ส่งทิศทางการหมุน มีค่าระหว่าง 1-3
  Wire.write(PWM); //ส่งความเร็วมอเตอร์ มีค่าระหว่าง 0-255
  Wire.endTransmission();
}

void loop() {
  // int LX = ps2x.Analog(PSS_LX);
  // int LY = ps2x.Analog(PSS_LY);
  // int RX = ps2x.Analog(PSS_RX);
  // int RY = ps2x.Analog(PSS_RY);
  if (ps2x.ButtonPressed(PSB_L2)) {
    lift(200);
  }
  if (ps2x.ButtonPressed(PSB_R2)){
    lift(-200);
  }
  if (ps2x.ButtonPressed(PSB_L1)) { //หุบ
    myservo.attach(0); 
    myservo.write(100);
  }
  if (ps2x.ButtonPressed(PSB_R1)){ //อ้า
    myservo.attach(0); 
    myservo.write(70);
  }
   ps2x.read_gamepad(false, vibrate);

  checkPS2Connection();
  // Motion control
  padCounter();
  padControl(selectOn);


  delay(10);
}
