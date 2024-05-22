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