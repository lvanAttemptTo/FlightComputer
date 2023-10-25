#include <Wire.h>
#include <math.h>
#include <SPI.h>
#include <SD.h>
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

int8_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
double pitch, roll;
int RED_LED = 5;
int BLUE_LED = 6;
int GREEN_LED = 7;

Adafruit_LPS22 lps;
float startPressure;

int grabs = 0;

File dataFile;
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

}
void loop()
{
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true);
  int AcXoff, AcYoff, AcZoff, GyXoff, GyYoff, GyZoff;
  int tempAcc, toff;
  double t, tx, tf;
  //Acceleration data correction
  // AcXoff = -194;
  // AcYoff = -30;
  // AcZoff = -1857;
  //Temperature correction
  toff = 0;
  //Gyro correction
  // GyXoff = 59;
  // GyYoff = 3;
  // GyZoff = 6;
  //read accel data
  AcX = (Wire.read() << 8 | Wire.read());
  AcY = (Wire.read() << 8 | Wire.read());
  AcZ = (Wire.read() << 8 | Wire.read());
  //read temperature data
  tempAcc = (Wire.read() << 8 | Wire.read());
  tx = tempAcc;
  // t = tx / 340 + 36.53;
  // tf = (t * 9 / 5) + 32;
  //read gyro data
  GyX = (Wire.read() << 8 | Wire.read());
  GyY = (Wire.read() << 8 | Wire.read());
  GyZ = (Wire.read() << 8 | Wire.read());
  //get pitch/roll
  //getAngle(AcX, AcY, AcZ);

  // write to sd card(MOSI-pin 11, MISO-pin 12, CLK-pin 13, CS-pin 4)

  // Get baro data
  sensors_event_t temp;
  sensors_event_t pressure;
  lps.getEvent(&pressure, &temp);// get pressure

  //send the data out the serial port
  
  Serial.print(AcX);
  Serial.print(" ");
  Serial.print(AcY);
  Serial.print(" ");
  Serial.print(AcZ);
  Serial.print(" ");
  Serial.print(GyX);
  Serial.print(" ");
  Serial.print(GyY);
  Serial.print(" ");
  Serial.print(GyZ);
  Serial.print(" ");
  Serial.print(tx);
  Serial.print(" ");
  Serial.print(temp.temperature);
  Serial.print(" ");
  Serial.println(pressure.pressure);
  
  // float secs = micros();
  // grabs += 1;
  // Serial.print(secs);
  // Serial.print(" ");
  // Serial.println(grabs);


  //  Serial.print(",AX:");
  //  Serial.print(float(AcX)/2048);
  //  Serial.print(",AY:");
  //  Serial.print(float(AcY)/2048);
  //  Serial.print(",AZ:");
  //  Serial.print(float(AcZ)/2048);
  //  Serial.print(",GX:");
  //  Serial.print(float(GyX)/16.384);
  //  Serial.print(",GY:");
  //  Serial.print(float(GyY)/16.384);
  //  Serial.print(",GZ:");
  //  Serial.print(float(GyZ)/16.384);
  //  Serial.println();

  // long mil = millis();
  // long step = mil - prevMil;
  // prevMil = mil;

  // if (abs(AcZ) > 40)
  // {
  //   float VChange = ((float(AcZ)/2048) * (step));
  //   VVel  = VVel + VChange;
  // }
  // else
  // {
  //   noChangeZ = noChangeZ + 1;
  // }

  // if (abs(AcY) > 20)
  // {
  //   float FChange = ((float(AcY)/2048) * (step));
  //   FVel  = FVel + FChange;
  // }
  // else
  // {
  //   noChangeY = noChangeY + 1;
  // }

  // FPos = FPos + (FVel * step);

  // VPos = VPos + (VVel * step);
  // if (noChangeZ > 3)
  // {
  //   VVel = 0;
  //   noChangeZ = 0;
  // }
  // if (noChangeY > 3)
  // {
  //   FVel = 0;
  //   noChangeY = 0;
  // }

  // Serial.print("FPos:");
  // Serial.print(FPos);
  // Serial.print(",VPos:");
  // Serial.println(FVel);
}
//convert the accel data to pitch/roll
// void getAngle(int Vx, int Vy, int Vz) {
//  double x = Vx;
//  double y = Vy;
//  double z = Vz;
//  pitch = atan(x / sqrt((y * y) + (z * z)));
//  roll = atan(y / sqrt((x * x) + (z * z)));
//  //convert radians into degrees
//  pitch = pitch * (180.0 / 3.14);
//  roll = roll * (180.0 / 3.14);
// }