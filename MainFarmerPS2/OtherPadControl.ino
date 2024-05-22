int stepPerRevolve = 20;
void liftControl() {
  if (check_LY) {
    LiftMotor(50);
  } else if (!check_LY) {
    LiftMotor(-50);
  }

}
void conveyorControl() {
  if (check_LX) {
    conveyorMotor(50);
  } else if (!check_LX) {
    conveyorMotor(-50);
  }

}
void shootMotorControl(){
  if (ps2x.Button(PSB_CIRCLE)) {
    shootMotor(255);
  }
}
void stepperControl(){
  if(ps2x.Button(PSB_L1)){
    stepperMoveControl(1, 500, stepPerRevolve);
  }else if(ps2x.Button(PSB_R1)){
    stepperMoveControl(0, 500, stepPerRevolve);
  }
}
void ballLiftControl(){
  if(ps2x.Button(PSB_R2)){
    if(rotaryEncoder(&counter,&currentState,&lastState)<=lowerEncoderLimit){
      moveBallUp=1;
    }else if(rotaryEncoder(&counter,&currentState,&lastState)>=upperEncoderLimit){
      moveBallUp=2;
    }
  }
  if(moveBallUp==1){
    BallGripperMotor(100);
    moveBallUp=0;
  }else if(moveBallUp==2){
    BallGripperMotor(-100);
    moveBallUp=0;
  }

}
void riceGripperControl(){
  if (ps2x.Button(PSB_CROSS)) {
    if(riceOpen){
      riceServo.write(0);
      riceServo2.write(180); 
    }else{
      riceServo.write(10);
      riceServo2.write(170); 
    }
    riceOpen = !riceOpen;
  }
}
void ballGripperControl(){
  if (ps2x.Button(PSB_TRIANGLE)) {
    if(gripperOpen){
      gripperServo.write(0);
      gripperServo2.write(180); 
    }else{
      gripperServo.write(10);
      gripperServo2.write(170);
    }
    gripperOpen = !gripperOpen;
  }
}