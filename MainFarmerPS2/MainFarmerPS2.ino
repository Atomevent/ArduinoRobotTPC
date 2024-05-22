#include <PS2X_lib.h>
#include <Stepper.h>
#include <Servo.h>
#include <Wire.h>

#define Left_I2C_ADDRESS 0x50
#define Right_I2C_ADDRESS 0x57
#define ConveyorBallGripper_I2C_ADDRESS 0x51
#define LiftBallShoot_I2C_ADDRESS 0x52
#define First_ADDRESS 0x85
#define Second_ADDRESS 0x86


//เปลี่ยนไปใช้ arduino mega แล้วจร้าาา
int buildInLED = 13;
#define PS2_DAT 12
#define PS2_CMD 11
#define PS2_CLK 10
#define PS2_ATT 9
#define stepPin 26
#define dirPin 27
#define shooterPin 8
#define ricePin 30
#define ricePin2 31
#define gripperPin 32
#define gripperPin2 33

// motor1 => 0x85 (front), motor2 => 0x86 (back)
Servo riceServo;
Servo riceServo2;
Servo gripperServo;
Servo gripperServo2;
bool liftBreak = 0;
unsigned long currentTimeLED = millis();
unsigned long currentTimeStepper = micros();
unsigned long currentforward = millis();
unsigned long currentbackward = millis();
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
bool check_LY = 0;
bool check_LX = 0;
int moveBallUp=0;
int upperEncoderLimit = 80;
int lowerEncoderLimit = 20;
bool stepperMoving;
int swState = 0;
int counter = 0; 
int currentState;
int lastState;
#define outputA 2
#define outputB 3
#define sw 4
#define pressures false
#define rumble false
bool gripperOpen =0;
bool riceOpen = 0;
int error = 0;
byte type = 0;
byte vibrate = 0;

int prevDir = 3;



PS2X ps2x; 
 // create PS2 Controller Class
void setup() {
  pinMode(buildInLED, OUTPUT);
  pinMode(PS2_ATT, OUTPUT);
  pinMode(PS2_CLK, OUTPUT);
  pinMode(PS2_CMD, OUTPUT);
  pinMode(PS2_DAT, OUTPUT);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(outputA, INPUT);
  pinMode(outputB, INPUT);
  pinMode(shooterPin, OUTPUT);


  riceServo.attach(ricePin);
  riceServo2.attach(ricePin2);
  gripperServo.attach(gripperPin);
  gripperServo2.attach(gripperPin2);
  Serial.begin(57600);
  Wire.begin();
  delay(500);
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT, pressures, rumble);

  // Serial.println(error);
  
  Serial.println("start");
}



void checkPS2Connection() {
  if (millis() - currentTimeLED > 100) {
    currentTimeLED = millis();
    digitalWrite(buildInLED, !digitalRead(buildInLED));  
    if (!ps2x.readType()) { // check if controller is disconnected
      Serial.println("PS2 controller disconnected!");
    }
    else {
      Serial.println("Hi");
    }

  }
}

// Stepper motor





void loop() {
  
  int LX = ps2x.Analog(PSS_LX);
  int LY = ps2x.Analog(PSS_LY);
  int RX = ps2x.Analog(PSS_RX);
  int RY = ps2x.Analog(PSS_RY);
  // Serial.println(LX);

  checkPS2Connection();
  // Motion control
  // Serial.println( true_RX);
  padControl(selectOn);
  
  liftControl();
  conveyorControl();
  shootMotorControl();
  stepperControl();
  ballLiftControl();

  


  // Stepper motor
  // if (ps2x.Button(PSB_SQUARE)) stepperControl(HIGH, 1000, 100);
  // //  else if (ps2x.ButtonReleased(PSB_SQUARE)) stepperControl(HIGH,0,0);
  // if (ps2x.Button(PSB_CIRCLE)) stepperControl(LOW, 1000, 100);
  // //  else if (ps2x.ButtonReleased(PSB_CIRCLE)) stepperControl(LOW,0,0);


  delay(200);
}