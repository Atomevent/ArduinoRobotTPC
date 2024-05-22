void padControl(int Speed) {

  //if (currentforward >= millis()) moveForward(Speed);
  //else if (currentbackward >= millis()) moveBackward(Speed);

  // else if (ps2x.Button(PSB_PAD_UP) && ps2x.Button(PSB_PAD_LEFT)) forwardLeft(Speed);
  // else if (ps2x.Button(PSB_PAD_UP) && ps2x.Button(PSB_PAD_RIGHT)) forwardRight(Speed);
  // else if (ps2x.Button(PSB_PAD_DOWN) && ps2x.Button(PSB_PAD_LEFT)) backwardLeft(Speed);
  // else if (ps2x.Button(PSB_PAD_DOWN) && ps2x.Button(PSB_PAD_RIGHT)) backwardRight(Speed);
  // Serial.println("Mdsi");

  // Serial.println("Hi");
  check_RY = 0;
  check_RX = 0;
  check_LX=0;
  check_LY=0;
  true_RX=0;
  true_RY=0;

  if(ps2x.Analog(PSS_LY) >= 150 )//real center value is 128, but 140 is needed because controller is HIGHLY sensitive
  {
    check_LY = 0;
  }
  else if(ps2x.Analog(PSS_LY) <= 100) //Same as above
  {
    check_LY = 1;// create a set range for values and set output values respectively
  }  
  if(ps2x.Analog(PSS_LX) >= 150 )//real center value is 128, but 140 is needed because controller is HIGHLY sensitive
  {
    check_LX = 0;
  }
  else if(ps2x.Analog(PSS_LX) <= 114) //Same as above
  {
    check_LX = 1;
  }  


  if(ps2x.Analog(PSS_RY) >= 150 )//real center value is 128, but 140 is needed because controller is HIGHLY sensitive
  {
    true_RY = map(ps2x.Analog(PSS_RY),150 , 255, 0, -Speed);
    check_RY = 0;
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
    check_RX = 0;
    true_RX = map(ps2x.Analog(PSS_RX),0 ,114 , -Speed , 0);// create a set range for values and set output values respectively
  }  
  if(ps2x.ButtonPressed(PSB_R3)){
    if(speedFraction == 1){
      speedFraction = 0.4;
    }else{
      speedFraction = 1;
    }
  }
  // Serial.println("Here2");
  if(ps2x.Button(PSB_PAD_LEFT )){
    Serial.print("L");
    slideLeft(Speed);
  }else if(ps2x.Button(PSB_PAD_RIGHT )){
    slideRight(Speed);
    Serial.print("Right");
  }else if(ps2x.Button(PSB_PAD_UP )){
    forward(Speed);
    Serial.print("Up");
  }else if(ps2x.Button(PSB_PAD_DOWN )){
    backward(Speed);
    Serial.print("Dowsn");
  }else if(!check_RX&&!check_RY){
    StopMove();   
    Serial.print("Wow");
  } 
  // Serial.println("padControl ENded");
  moveFromJoystick(true_RX,true_RY);


}