#define outputA 2
#define outputB 3
float counter = 0; 
float c = 22/7*10;
float d = c/2000;

void setup() {
  pinMode(outputA, INPUT_PULLUP);
  pinMode(outputB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(outputA),readEncoder,RISING);
  Serial.begin(57600);
}
void readEncoder(){
  int b = digitalRead(outputB);
  if(b==1){
    counter++;
  }else{
    counter--;
  }
}
void loop() {
  int a = digitalRead(outputA);
  int b = digitalRead(outputB);
  Serial.println(counter*d);
}
