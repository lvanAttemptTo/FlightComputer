#include <Wire.h>
#include <math.h>
#include <SPI.h>
#include <Adafruit_LPS2X.h>
#include <Adafruit_Sensor.h>
const int MPU = 0x68;



// long prevMil = 0;
// float VPos = 0;
// float VVel = 0;
// int noChangeZ = 0;
// float FPos = 0;
// float FVel = 0;
// int noChangeY = 0;

int16_t AcX[2], AcY[2], AcZ[2], GyX[2], GyY[2], GyZ[2], tx[2];
float pres[2], tmp[2];
uint8_t dataPointer = 0;
bool launched = false;
int8_t largestDif = 0;

Adafruit_LPS22 lps;

int grabs = 0;

void setup()
{


  Wire.begin();
  Wire.setClock(400000L);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // Set MPU6050 to +- 16g
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);  //pointing Register-28
  Wire.endTransmission();


  Wire.requestFrom(MPU, 1);
  byte x = Wire.read();  //the value of Register-28 is in x


  x = x | 0b00011000;  //appending values of Bit4 and Bit3


  Wire.beginTransmission(MPU);
  Wire.write(0x1C);  //pointing Register-28
  Wire.write(x);     //value for Register-28; Full-scale range is now +/- 16g
  Wire.endTransmission();

  // Set MPU6050 to +- 2000 deg/s
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);  //pointing Register-28
  Wire.endTransmission();


  Wire.requestFrom(MPU, 1);
  byte y = Wire.read();  //the value of Register-28 is in x


  y = y | 0b00011000;  //appending values of Bit4 and Bit3


  Wire.beginTransmission(MPU);
  Wire.write(0x1B);  //pointing Register-28
  Wire.write(y);     //value for Register-28; Full-scale range is now +/- 2000 deg/s
  Wire.endTransmission();
  Serial.begin(2000000);
  while (!Serial) delay(10);

  if (!lps.begin_I2C())
  {
      Serial.println("No baromenter");
      while (1) delay(10);
  }

  lps.setDataRate(LPS22_RATE_75_HZ);
  dataPointer = (dataPointer + 1) % 2;
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true);



  //read accel data
  AcX[dataPointer] = (Wire.read() << 8 | Wire.read());
  AcY[dataPointer] = (Wire.read() << 8 | Wire.read());
  AcZ[dataPointer] = (Wire.read() << 8 | Wire.read());
  //read temperature data
  tx[dataPointer] = (Wire.read() << 8 | Wire.read());

  // t = tx / 340 + 36.53;
  // tf = (t * 9 / 5) + 32;
  //read gyro data
  GyX[dataPointer] = (Wire.read() << 8 | Wire.read());
  GyY[dataPointer] = (Wire.read() << 8 | Wire.read());
  GyZ[dataPointer] = (Wire.read() << 8 | Wire.read());
  if (AcX[1] < 500 && AcY[1] < 500 && AcZ[1] < 500)
  {
    Serial.println("No Accelerometer");
    while (1) delay(10);

  }

}
void loop()
{
  while (launched == false)
  {
    dataPointer = (dataPointer + 1) % 2;
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 14, true);



    //read accel data
    AcX[dataPointer] = (Wire.read() << 8 | Wire.read());
    AcY[dataPointer] = (Wire.read() << 8 | Wire.read());
    AcZ[dataPointer] = (Wire.read() << 8 | Wire.read());
    //read temperature data
    tx[dataPointer] = (Wire.read() << 8 | Wire.read());

    // t = tx / 340 + 36.53;
    // tf = (t * 9 / 5) + 32;
    //read gyro data
    GyX[dataPointer] = (Wire.read() << 8 | Wire.read());
    GyY[dataPointer] = (Wire.read() << 8 | Wire.read());
    GyZ[dataPointer] = (Wire.read() << 8 | Wire.read());
    //get pitch/roll
    //getAngle(AcX, AcY, AcZ);

    // write to sd card(MOSI-pin 11, MISO-pin 12, CLK-pin 13, CS-pin 4)

    // Get baro data
    sensors_event_t temp;
    sensors_event_t pressure;
    lps.getEvent(&pressure, &temp);// get pressure
    pres[dataPointer] = pressure.pressure;
    tmp[dataPointer] = temp.temperature;

    //send the data out the serial port
    
    // Serial.print(AcX[dataPointer]);
    // Serial.print(" ");
    // Serial.print(AcY[dataPointer]);
    // Serial.print(" ");
    // Serial.print(AcZ[dataPointer]);
    // Serial.print(" ");
    // Serial.print(GyX[dataPointer]);
    // Serial.print(" ");
    // Serial.print(GyY[dataPointer]);
    // Serial.print(" ");
    // Serial.print(GyZ[dataPointer]);
    // Serial.print(" ");
    // Serial.print(tx[dataPointer]);
    // Serial.print(" ");
    // Serial.print(pres[dataPointer]);
    // Serial.print(" ");
    // Serial.println(tmp[dataPointer]);

    if(abs(AcX[0] - AcX[1]) > 1000)
    {
    Serial.print(AcX[0]);
    Serial.print(" ");
    Serial.print(AcY[0]);
    Serial.print(" ");
    Serial.print(AcZ[0]);
    Serial.print(" ");
    Serial.print(GyX[0]);
    Serial.print(" ");
    Serial.print(GyY[0]);
    Serial.print(" ");
    Serial.print(GyZ[0]);
    Serial.print(" ");
    Serial.print(tx[0]);
    Serial.print(" ");
    Serial.print(pres[0]);
    Serial.print(" ");
    Serial.println(tmp[0]);
    Serial.print(AcX[1]);
    Serial.print(" ");
    Serial.print(AcY[1]);
    Serial.print(" ");
    Serial.print(AcZ[1]);
    Serial.print(" ");
    Serial.print(GyX[1]);
    Serial.print(" ");
    Serial.print(GyY[1]);
    Serial.print(" ");
    Serial.print(GyZ[1]);
    Serial.print(" ");
    Serial.print(tx[1]);
    Serial.print(" ");
    Serial.print(pres[1]);
    Serial.print(" ");
    Serial.println(tmp[1]);
    launched = true;
    }
    if(1000 < abs(AcY[0] - AcY[1]))
    {
      Serial.print(AcX[0]);
    Serial.print(" ");
    Serial.print(AcY[0]);
    Serial.print(" ");
    Serial.print(AcZ[0]);
    Serial.print(" ");
    Serial.print(GyX[0]);
    Serial.print(" ");
    Serial.print(GyY[0]);
    Serial.print(" ");
    Serial.print(GyZ[0]);
    Serial.print(" ");
    Serial.print(tx[0]);
    Serial.print(" ");
    Serial.print(pres[0]);
    Serial.print(" ");
    Serial.println(tmp[0]);
    Serial.print(AcX[1]);
    Serial.print(" ");
    Serial.print(AcY[1]);
    Serial.print(" ");
    Serial.print(AcZ[1]);
    Serial.print(" ");
    Serial.print(GyX[1]);
    Serial.print(" ");
    Serial.print(GyY[1]);
    Serial.print(" ");
    Serial.print(GyZ[1]);
    Serial.print(" ");
    Serial.print(tx[1]);
    Serial.print(" ");
    Serial.print(pres[1]);
    Serial.print(" ");
    Serial.println(tmp[1]);
    launched = true;

    }
    if(1000 < abs(AcZ[0] - AcZ[1]))
    {
      Serial.print(AcX[0]);
    Serial.print(" ");
    Serial.print(AcY[0]);
    Serial.print(" ");
    Serial.print(AcZ[0]);
    Serial.print(" ");
    Serial.print(GyX[0]);
    Serial.print(" ");
    Serial.print(GyY[0]);
    Serial.print(" ");
    Serial.print(GyZ[0]);
    Serial.print(" ");
    Serial.print(tx[0]);
    Serial.print(" ");
    Serial.print(pres[0]);
    Serial.print(" ");
    Serial.println(tmp[0]);
    Serial.print(AcX[1]);
    Serial.print(" ");
    Serial.print(AcY[1]);
    Serial.print(" ");
    Serial.print(AcZ[1]);
    Serial.print(" ");
    Serial.print(GyX[1]);
    Serial.print(" ");
    Serial.print(GyY[1]);
    Serial.print(" ");
    Serial.print(GyZ[1]);
    Serial.print(" ");
    Serial.print(tx[1]);
    Serial.print(" ");
    Serial.print(pres[1]);
    Serial.print(" ");
    Serial.println(tmp[1]);
    launched = true;
    }
    

    



  }

}
