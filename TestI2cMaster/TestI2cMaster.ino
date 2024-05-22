#include <SPI.h>
int x = 5;
byte hi,lo;
#define slaveB 53
void setup()
{
  Serial.begin(57600);
  SPI.begin();
  
}



void loop()
{
  lo = lowByte(x);
  hi = highByte(x);
  byte byteValue[2] = {lo,hi};
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
  digitalWrite (slaveB, LOW);
  Serial.print(byteValue[0]);
  Serial.println(byteValue[1]);
  SPI.transfer(byteValue, sizeof(byteValue));
  digitalWrite (slaveB, HIGH);
  SPI.endTransaction();
}