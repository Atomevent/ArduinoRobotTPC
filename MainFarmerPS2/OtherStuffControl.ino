void stepperMoveControl(bool dir, int Speed, int steps) {
  digitalWrite(dirPin, dir);
  int i = 0;
  stepperMoving =1;
  while (i < steps&&!stepperMoving) {
    if (micros() - currentTimeStepper > Speed) {
      currentTimeStepper = micros();
      digitalWrite(stepPin, !digitalRead(stepPin));
      i++;
    }
  }
  stepperMoving =0;
}