#include <Stepper.h>
#include <Servo.h>
#include <Wire.h>
#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "le3dp_rptparser.h"
//เปลี่ยนไปใช้ arduino mega แล้วจร้าาา
int buildInLED = 13;
#define stepPin 26
#define dirPin 27
#define shooterPin 8
#define Left_I2C_ADDRESS 0x50
#define Right_I2C_ADDRESS 0x57
#define lift_I2C_ADDRESS 0x51
#define liftMotorPwm 1
#define liftMotorIn1 2
#define liftMotorIn2 3

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);

volatile int ledPin = 0;
volatile byte F710Slider = 0x08; // Default value
volatile boolean beepFlag = false;
byte F710SliderLeft = 0x00; // Direction
byte F710SliderRight = 0x00; // Buttons
boolean F710ButtonX = false;
boolean F710ButtonA = false;
boolean F710ButtonB = false;
boolean F710ButtonY = false;
// motor1 => 0x85 (front), motor2 => 0x86 (back)


bool liftBreak = 0;
int selectOn = 150;
float speed1=0;
float speed2=0;
int RollerSpeed = 0;
float speedFraction = 1;
float true_RY = 0;
float true_RX = 0;
float old_front_left = 0;
float old_front_right = 0;
float old_back_left = 0;
float old_back_right = 0;
bool check_RY = 0;
bool check_RX = 0;
#define encoderA 2
#define encoderB 3
#define sw 4
int swState = 0;
int counter = 0; 
float currentTimeStepper =0;
int currentState;
int lastState;
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
  int newSpeed = Speed *speedFraction;
  old_front_left = newSpeed;
  controlMotor(newSpeed, &PWM, &Dir);
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
  int newSpeed = Speed *speedFraction;
  old_back_left = newSpeed;
  controlMotor(newSpeed, &PWM, &Dir);
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
  int newSpeed = Speed *speedFraction;
  old_front_right = newSpeed;
  controlMotor(newSpeed, &PWM, &Dir);
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
  int newSpeed = Speed *speedFraction;
  old_back_right = newSpeed;
  controlMotor(newSpeed, &PWM, &Dir);
  prevDir = Dir;
  Wire.beginTransmission(Right_I2C_ADDRESS);  //เริ่มส่งข้อมูลไขยัง Address ที่กำหนดไว้ใน MOIOR_ I2C_ADDRESS
  Wire.write(0x86);
  Wire.write(Dir);  //ส่งทิศทางหมุนสำหรับมอเตอร์ตัวที่ 1 ซึ่งมีค่าเท่ากับ 1-3
  Wire.write(PWM);  //ส่ง Command : Motor 1 control
  Wire.endTransmission();
}
void moveAllMotor(int frontLeft,int frontRight,int backLeft,int backRight){
  if(frontLeft * old_front_left < 0 || frontRight * old_front_right<0 || backLeft * old_back_left<0 || backRight * old_back_right<0){
    Stop();
    delay(100);
    return;
  }
  frontLeftMotor(frontLeft);
  frontRightMotor(frontRight);
  backLeftMotor(backLeft);
  backRightMotor(backRight);
}

void setup() {
  pinMode(buildInLED, OUTPUT);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  pinMode(shooterPin, OUTPUT);

  pinMode(liftMotorPwm, OUTPUT);
  pinMode(liftMotorIn1, OUTPUT);
  pinMode(liftMotorIn2, OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  #if !defined(__MIPSEL__)
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
  #endif
  Serial.println("Start");

  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");

  if (!Hid.SetReportParser(0, &Joy))
    ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1  );

  delay(1000);
  Serial.println("start");
}

void moveFromJoystick(float trueX,float trueY){
  if(check_RX&&check_RY){
    if(trueY>0){
      if(trueX>0){
        if(trueY>trueX){
          int newSpeed = map(trueY-trueX,150 ,0, 150, 80);
          moveAllMotor(trueY,trueY,newSpeed,newSpeed);
        }else{
          int newSpeed = map(trueX-trueY,150 ,0, 10, 80);
          moveAllMotor(trueX,trueX,newSpeed,newSpeed);
        }
      }else{
        if(abs(trueY)>abs(trueX)){
          int newSpeed = map(abs(trueY)-abs(trueX),150 ,0, 150, 80);
          moveAllMotor(newSpeed,newSpeed,trueY,trueY);
        }else{
          int newSpeed = map(abs(trueX)-abs(trueY),150 ,0, 10, 80);
          moveAllMotor(newSpeed,newSpeed,-trueX,-trueX);
        }
      }   
    }else{
      if(trueX>0){
        if(abs(trueY)>abs(trueX)){
          int newSpeed = map(abs(trueY)-abs(trueX),150 ,0, -150, -80);
          moveAllMotor(trueY,trueY,newSpeed,newSpeed);
        }else{
          int newSpeed = map(abs(trueX)-abs(trueY),150 ,0, -10, -80);
          moveAllMotor(trueX,trueX,newSpeed,newSpeed);
        }
      }else{
        if(abs(trueY)>abs(trueX)){
          int newSpeed = map(abs(trueY)-abs(trueX),150 ,0, -150, -80);
          moveAllMotor(newSpeed,newSpeed,trueY,trueY);
        }else{
          int newSpeed = map(abs(trueX)-abs(trueY),150 ,0, -10, -80);
          moveAllMotor(newSpeed,newSpeed,trueX,trueX);
        }
          
      } 
    }
  }else if(check_RX&&!check_RY){
    moveAllMotor(trueX,trueX,-trueX,-trueX);
  }else if(!check_RX&&check_RY){
    moveAllMotor(trueY,trueY,-trueY,-trueY);
  }
}
void forward(int Speed) {
  frontLeftMotor(Speed);
  backLeftMotor(Speed);
  frontRightMotor(Speed);
  backRightMotor(Speed);
}
void backward(int Speed) {
  frontLeftMotor(-Speed);
  backLeftMotor(-Speed);
  frontRightMotor(-Speed);
  backRightMotor(-Speed);
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

void Stop() {
  frontLeftMotor(0);
  backLeftMotor(0);
  frontRightMotor(0);
  backRightMotor(0);
}

void JoystickEvents::OnGamePadChanged(const GamePadEventData *evt)
{
  Serial.print("X: ");
  PrintHex<uint16_t>(evt->x, 0x80);
  Serial.print(" Y: ");
  PrintHex<uint16_t>(evt->y, 0x80);
  Serial.print(" Hat Switch: ");
  PrintHex<uint8_t>(evt->hat, 0x80);
  Serial.print(" Twist: ");
  PrintHex<uint8_t>(evt->twist, 0x80);
  Serial.print(" Slider: ");
  F710Slider = evt->slider;
  PrintHex<uint8_t>(F710Slider, 0x80);
  Serial.print(" Buttons A: ");
  PrintHex<uint8_t>(evt->buttons_a, 0x80);
  Serial.print(" Buttons B: ");
  PrintHex<uint8_t>(evt->buttons_b, 0x80);
  Serial.println("");
}


// void padControl(int Speed) {

//   //if (currentforward >= millis()) moveForward(Speed);
//   //else if (currentbackward >= millis()) moveBackward(Speed);

//   //else if (ps2x.Button(PSB_PAD_UP) && ps2x.Button(PSB_PAD_LEFT)) forwardLeft(Speed);
//   //else if (ps2x.Button(PSB_PAD_UP) && ps2x.Button(PSB_PAD_RIGHT)) forwardRight(Speed);
//   //else if (ps2x.Button(PSB_PAD_DOWN) && ps2x.Button(PSB_PAD_LEFT)) backwardLeft(Speed);
//   //else if (ps2x.Button(PSB_PAD_DOWN) && ps2x.Button(PSB_PAD_RIGHT)) backwardRight(Speed);


//   // Serial.println("Hi");
//   check_RY = 0;
//   check_RX = 0;
//   true_RX=0;
//   true_RY=0;
  
//   if(ps2x.Analog(PSS_RY) >= 150 )//real center value is 128, but 140 is needed because controller is HIGHLY sensitive
//   {
//     true_RY = map(ps2x.Analog(PSS_RY),150 , 255, 0, -Speed);
//     check_RY = 1;
//   }
//   else if(ps2x.Analog(PSS_RY) <= 100) //Same as above
//   {
//     true_RY = map(ps2x.Analog(PSS_RY),0 ,114 , Speed, 0);
//     check_RY = 1;// create a set range for values and set output values respectively
//   }  
//   if(ps2x.Analog(PSS_RX) >= 150 )//real center value is 128, but 140 is needed because controller is HIGHLY sensitive
//   {
//     check_RX = 1;
//     true_RX = map(ps2x.Analog(PSS_RX),150 , 255, 0 , Speed);
//   }
//   else if(ps2x.Analog(PSS_RX) <= 114) //Same as above
//   {
//     check_RX = 1;
//     true_RX = map(ps2x.Analog(PSS_RX),0 ,114 , -Speed , 0);// create a set range for values and set output values respectively
//   }  
//   if(ps2x.ButtonPressed(PSB_L3)){
//     if(speedFraction == 1){
//       speedFraction = 0.4;
//     }else{
//       speedFraction = 1;
//     }
//   }
//   if(ps2x.Button(PSB_PAD_LEFT )){
//     slideLeft(Speed);
//     return;
//   }else if(ps2x.Button(PSB_PAD_RIGHT )){
//     slideRight(Speed);
//     return;
//   }else if(ps2x.Button(PSB_PAD_UP )){
//     forward(Speed);
//     return;
//   }else if(ps2x.Button(PSB_PAD_DOWN )){
//     backward(Speed);
//     return;
//   }else if(!check_RX&&!check_RY){
//     Stop();
//     return;
//   }

//   moveFromJoystick(true_RX,true_RY);
    




  
// }
int rotaryEncoder(int outputA,int outputB,int *counter,int *currentState,int *lastState){
  currentState = digitalRead(outputA);
  if (currentState != lastState){     
    if (digitalRead(outputB) != currentState) { 
      counter ++;
    }
    else{
      counter --;
    }
    Serial.print("Position: ");
    Serial.println(*counter);
  }
  lastState = currentState;
  
  return counter;
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
  // if (ps2x.Button(PSB_TRIANGLE)) {
  //   analogWrite(liftMotorIn1, 80);
  //   analogWrite(liftMotorIn2, 0);
  // } else if (ps2x.Button(PSB_CROSS)) {
  //   analogWrite(liftMotorIn1, 0);
  //   analogWrite(liftMotorIn2, 50);
  // }

  // else if (liftBreak) {
  //   analogWrite(liftMotorIn1, 255);
  //   analogWrite(liftMotorIn2, 255);
  // } else {
  //   analogWrite(liftMotorIn1, 0);
  //   analogWrite(liftMotorIn2, 0);
  // }

  // if (ps2x.ButtonPressed(PSB_SELECT)) {
  //   if (liftBreak) liftBreak = 0;
  //   else liftBreak = 1;
  // }
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
void conveyor(int Speed){

}
void pickupCrop(){

}
void openChute(){
  
}
void openMotor(){

}
void pickupBall(){

}
void shootBall(){


}
void loop() {
  // int LX = ps2x.Analog(PSS_LX);
  // int LY = ps2x.Analog(PSS_LY);
  // int RX = ps2x.Analog(PSS_RX);
  // int RY = ps2x.Analog(PSS_RY);
  // if (ps2x.ButtonPressed(PSB_L2)) {
  //   lift(200);
  // }
  // if (ps2x.ButtonPressed(PSB_R2)){
  //   lift(-200);
  // }
  
  // Motion control
  Usb.Task();
  // padControl(selectOn);
  




  // Stepper motor
  // if (ps2x.Button(PSB_SQUARE)) stepperControl(HIGH, 1000, 100);
  // //  else if (ps2x.ButtonReleased(PSB_SQUARE)) stepperControl(HIGH,0,0);
  // if (ps2x.Button(PSB_CIRCLE)) stepperControl(LOW, 1000, 100);
  // //  else if (ps2x.ButtonReleased(PSB_CIRCLE)) stepperControl(LOW,0,0);

  // if (ps2x.Button(PSB_R2) ){
  //   RollerSpeed = 200;
  //   analogWrite(shooterPin, RollerSpeed);
  // }

  // else if (ps2x.Button(PSB_R1)) {

  //   // if(ps2x.ButtonPressed(PSB_SQUARE))
  //   //   RollerSpeed = 0;
  //   // else if(ps2x.ButtonPressed(PSB_CROSS))
  //   //   RollerSpeed = 68;
  //   // else if(ps2x.ButtonPressed(PSB_CIRCLE))
  //   //   RollerSpeed = 128;
  //   if (ps2x.ButtonPressed(PSB_SQUARE)) {
  //     RollerSpeed = 0;
  //   } else if (ps2x.ButtonPressed(PSB_CROSS)) {
  //     RollerSpeed -= 24;
  //   } else if (ps2x.ButtonPressed(PSB_CIRCLE)) {
  //     RollerSpeed += 24;
  //   } else if (ps2x.ButtonPressed(PSB_TRIANGLE)) {
  //     RollerSpeed += 128;
  //   }
  //   if (RollerSpeed > 128) {
  //     RollerSpeed = 128;
  //   } else if (RollerSpeed < 0) {
  //     RollerSpeed = 0;
  //   }
  //   analogWrite(shooterPin, RollerSpeed);
  // } else {
  //   // Stepper motor
  //   if (ps2x.Button(PSB_SQUARE)) stepperControl(HIGH, 4000, 5);
  //   //  else if (ps2x.ButtonReleased(PSB_SQUARE)) stepperControl(HIGH,0,0);
  //   if (ps2x.Button(PSB_CIRCLE)) stepperControl(LOW, 4000, 5);
  //   //300 50
  // }

  delay(10);
}