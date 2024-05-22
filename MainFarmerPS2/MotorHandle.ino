
int shootMotorSpeed =255;
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
void BallGripperMotor(int Speed){
  int PWM;
  int Dir;
  int newSpeed = Speed *speedFraction;
  controlMotor(newSpeed, &PWM, &Dir);
  prevDir = Dir;
  Wire.beginTransmission(ConveyorBallGripper_I2C_ADDRESS);  //เริ่มส่งข้อมูลไขยัง Address ที่กำหนดไว้ใน MOIOR_ I2C_ADDRESS
  Wire.write(0x86);
  Wire.write(Dir);  //ส่งทิศทางการหมุน มีค่าระหว่าง 1-3
  Wire.write(PWM);  //ส่งความเร็วมอเตอร์ มีค่าระหว่าง 0-255
  Wire.endTransmission();
}
void LiftMotor(int Speed){
  int PWM;
  int Dir;
  int newSpeed = Speed *speedFraction;
  controlMotor(newSpeed, &PWM, &Dir);
  prevDir = Dir;
  Wire.beginTransmission(LiftBallShoot_I2C_ADDRESS);  //เริ่มส่งข้อมูลไขยัง Address ที่กำหนดไว้ใน MOIOR_ I2C_ADDRESS
  Wire.write(0x85);
  Wire.write(Dir);  //ส่งทิศทางการหมุน มีค่าระหว่าง 1-3
  Wire.write(PWM);  //ส่งความเร็วมอเตอร์ มีค่าระหว่าง 0-255
  Wire.endTransmission();
}
void shootMotor(int Speed){
  int PWM;
  int Dir;
  int newSpeed = Speed *speedFraction;
  controlMotor(newSpeed, &PWM, &Dir);
  prevDir = Dir;
  Wire.beginTransmission(LiftBallShoot_I2C_ADDRESS);  //เริ่มส่งข้อมูลไขยัง Address ที่กำหนดไว้ใน MOIOR_ I2C_ADDRESS
  Wire.write(0x86);
  Wire.write(Dir);  //ส่งทิศทางการหมุน มีค่าระหว่าง 1-3
  Wire.write(PWM);  //ส่งความเร็วมอเตอร์ มีค่าระหว่าง 0-255
  Wire.endTransmission();
}
void conveyorMotor(int Speed) {
  int PWM;
  int Dir;
  int newSpeed = Speed *speedFraction;
  controlMotor(newSpeed, &PWM, &Dir);
  prevDir = Dir;
  Wire.beginTransmission(ConveyorBallGripper_I2C_ADDRESS);  //เริ่มส่งข้อมูลไขยัง Address ที่กำหนดไว้ใน MOIOR_ I2C_ADDRESS
  Wire.write(0x85);
  Wire.write(Dir);  //ส่งทิศทางการหมุน มีค่าระหว่าง 1-3
  Wire.write(PWM);  //ส่งความเร็วมอเตอร์ มีค่าระหว่าง 0-255
  Wire.endTransmission();
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
int rotaryEncoder(int *counter,int *currentState,int *lastState){
  *currentState = digitalRead(outputA);
  if (*currentState != *lastState){     
    if (digitalRead(outputB) != *currentState) { 
      *counter ++;
    }
    else{
      *counter --;
    }
    // Serial.print("Position: ");
    // Serial.println(*counter);
  }
  *lastState = *currentState;
  
  return *counter;
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

void StopMove() {
  frontLeftMotor(0);
  backLeftMotor(0);
  frontRightMotor(0);
  backRightMotor(0);
}
void moveAllMotor(int frontLeft,int frontRight,int backLeft,int backRight){
  if(frontLeft * old_front_left < 0 || frontRight * old_front_right<0 || backLeft * old_back_left<0 || backRight * old_back_right<0){
    StopMove();
  }
  frontLeftMotor(frontLeft);
  frontRightMotor(frontRight);
  backLeftMotor(backLeft);
  backRightMotor(backRight);
}
