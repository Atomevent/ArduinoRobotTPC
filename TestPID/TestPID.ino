#include <util/atomic.h> 
#include <Wire.h>
#include <math.h>
const int minSpeed[] = {45,30,30,30}; //Fl Bl Fr Br
int lowestEncoderPos= 0;
/*------------ CLASS ------------*/
class SimplePID{
  private:
    float kp, kd, ki, umax;
    int pos;
    float eprev, eintegral;
    
  public:
    // Default initialization list
    SimplePID() : kp(0.01), kd(0.025), ki(0.001),umax(125),pos(0), eprev(0.0), eintegral(0.0) {}
    
    // Set the parameters
    void setParams(float kpIn, float kdIn, float kiIn, float umaxIn,int posIn){
      kp = kpIn; kd = kdIn; ki = kiIn; umax = umaxIn;pos = posIn;
    }

    // Evaluate the signal
    void evalu(int value, int target, float deltaT,int &pwr, int &dir){
        
      // error
      int e = target - value;
      
      float dedt = (e-eprev)/(deltaT);
      eintegral = eintegral + e*deltaT;
      float u = kp*e + kd*dedt + ki*eintegral;
      // motor power
      pwr = (int) fabs(u);

      if( pwr > umax ){
        pwr = umax;
      }else if(e <30){
        pwr = 0;
      }else if(pwr< minSpeed[pos]){
        pwr = minSpeed[pos];
      }
           
      // motor direction
      dir = 1;
      if(u<0){
        dir = -1;
      }
            
      // store previous error
      eprev = e;
    }
    
};
class SubPID{
  private:
    float kp, kd, ki;
    int pos;
    float eprev, eintegral;
    
  public:
    // Default initialization list
    SubPID() : kp(0.01), kd(0.01), ki(0.001),pos(0), eprev(0.0), eintegral(0.0) {}
    
    // Set the parameters
    void setParams(float kpIn, float kdIn, float kiIn,int posIn){
      kp = kpIn; kd = kdIn; ki = kiIn; pos=posIn;
    }

    // Evaluate the signal
    void evalu(int value, int target, float deltaT,int &output){
        
      // error
      int e = target - value;
      
      float dedt = (e-eprev)/(deltaT);
      eintegral = eintegral + e*deltaT;
      float u = kp*e + kd*dedt + ki*eintegral;
      // motor power
      output = u/400;
      Serial.println(output);
            
      // store previous error
      eprev = e;
    }
    
};
/*------------ GLOBALS AND DEFINITIONS ------------*/
// Define the motors
#define NMOTORS 4
#define M0 0
#define M1 1
const int enca[] = {2,18,3,19};
const int encb[]= {25,29,23,27};

// const int pwm[] = {9,13};
const int addr[] = {0x50,0x50,0x51,0x51};
const int motor[] = {0x85,0x86,0x85,0x86};

// Global variables
long prevT = 0;
int posPrev[] = {0,0};

// positions
long prev_target[]={0,0,0,0};
long target[] = {0,0,0,0};
volatile int Encoder_Target[] = {3333,3333,3333,3333};
// volatile int Encoder_Target[] = {1667,1667,1667,1667};
// volatile int Encoder_Target[] = {-3333,-3333,-3333,-3333};
// volatile int Encoder_Target[] = {500,500,500,500};
// volatile int Encoder_Target[] = {0,0,0,0};
volatile int Encoder_Val[] = {0,0,0,0};

// PID classes
SimplePID pid[NMOTORS];
SubPID subpid[NMOTORS];

/*------------ FUNCTIONS ------------*/
void setMotor(int dir, int pwmVal, int k){
  Wire.beginTransmission(addr[k]);  //เริ่มส่งข้อมูลไขยัง Address ที่กำหนดไว้ใน MOIOR_ I2C_ADDRESS
  Wire.write(motor[k]);
  if(dir == 1){
    Wire.write(1);  //ส่งทิศทางการหมุน มีค่าระหว่าง 1-3
    Wire.write(pwmVal);  //ส่งความเร็วมอเตอร์ มีค่าระหว่าง 0-255
  }
  else if(dir == -1){
    Wire.write(2);  //ส่งทิศทางการหมุน มีค่าระหว่าง 1-3
    Wire.write(pwmVal);  //ส่งความเร็วมอเตอร์ มีค่าระหว่าง 0-255
  }
  else{
    Wire.write(3);  //ส่งทิศทางการหมุน มีค่าระหว่าง 1-3
    Wire.write(pwmVal);  //ส่งความเร็วมอเตอร์ มีค่าระหว่าง 0-255
  }  
  Wire.endTransmission();
}

template <int j>
void readEncoder(){
  int b = digitalRead(encb[j]);
  if(encb[j]== 23||encb[j]==27){
    if(b > 0){
      Encoder_Val[j]--;
    }
    else{
      Encoder_Val[j]++;
    }
  }else{
    if(b > 0){
      Encoder_Val[j]++;
    }
    else{
      Encoder_Val[j]--;
    }
  }
  if(Encoder_Val[j]<Encoder_Val[lowestEncoderPos]){
    lowestEncoderPos = j;
  }
}

void sendLong(long value){
  for(int k=0; k<4; k++){
    byte out = (value >> 8*(3-k)) & 0xFF;
    Wire.write(out);
  }
}

long receiveLong(){
  long outValue;
  for(int k=0; k<4; k++){
    byte nextByte = Wire.read();
    outValue = (outValue << 8) | nextByte;
  }
  return outValue;
}

// targets


void setTarget(){

}

/*------------ SETUP ------------*/
void setup() {
  Wire.begin();        // join i2c bus
  Serial.begin(9600);
  for(int k = 0; k < NMOTORS; k++){
    pinMode(enca[k],INPUT_PULLUP);
    pinMode(encb[k],INPUT_PULLUP);
    pid[k].setParams(1,0.1,0,80,k);
    subpid[k].setParams(1,0.1,0,k);
  }
  attachInterrupt(digitalPinToInterrupt(enca[0]),readEncoder<0>,RISING);
  attachInterrupt(digitalPinToInterrupt(enca[1]),readEncoder<1>,RISING);
  attachInterrupt(digitalPinToInterrupt(enca[2]),readEncoder<2>,RISING);
  attachInterrupt(digitalPinToInterrupt(enca[3]),readEncoder<3>,RISING);


}

/*------------ LOOP ------------*/
void loop() {

  // time difference
  long currT = micros();
  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
  prevT = currT;
  // set target position
  setTarget();
  // Send the requested position to the follower
  // Get the current position from the follower

  // Read the position in an atomic block to avoid a potential misread 
  // Loop through the motors
  for(int k = 0; k < NMOTORS; k++){
    int pwr, dir,adjust;
    pid[k].evalu(Encoder_Val[k],Encoder_Target[k],deltaT,pwr,dir); 
    subpid[k].evalu(Encoder_Val[k],Encoder_Val[lowestEncoderPos],deltaT,adjust);
    if(pwr+adjust<30){
      pwr =0;
      adjust=0;
    }
    setMotor(dir,pwr+adjust,k); // signal the motor
  }

  for(int k=0; k<4; k++){
    Serial.print(Encoder_Val[k]);
    Serial.print(" ");
  }
  for(int k=0; k<4; k++){
    Serial.print(Encoder_Target[k]);
    Serial.print(" ");
  }
  Serial.println(" ");
}