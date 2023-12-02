#include <Wire.h>
#include <math.h>
#include <SPI.h>
#include <Adafruit_LPS2X.h>
const int MPU = 0x68;

#define CORE_0 0
#define CORE_1 1


// long prevMil = 0;
// float VPos = 0;
// float VVel = 0;
// int noChangeZ = 0;
// float FPos = 0;
// float FVel = 0;
// int noChangeY = 0;

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
sensors_event_t temp;
sensors_event_t pressure;
int tx;
// double pitch, roll;
long runs = 0;
long start = 0;
int RED_LED = 5;
int BLUE_LED = 6;
int GREEN_LED = 7;
Adafruit_LPS22 lps;
float startPressure;

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
 
 xTaskCreatePinnedToCore(
    readData, "Data Read", 2048  // Stack size
    ,
    NULL  // When no parameter is used, simply pass NULL
    ,
    1  // Priority
    ,
    NULL  // With task handle we will be able to manipulate with this task.
    ,
    CORE_0  // Core on which the task will run
  );
  xTaskCreatePinnedToCore(
    printData, "Data Print", 2048  // Stack size
    ,
    NULL  // When no parameter is used, simply pass NULL
    ,
    1  // Priority
    ,
    NULL  // With task handle we will be able to manipulate with this task.
    ,
    CORE_1  // Core on which the task will run
  );
  start = millis();
}

void loop() {

}
void readData(void *pvParameters)
{
  (void)pvParameters;

  for (;;) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 14, true);

    int tempAcc;

    //Acceleration data correction
    // AcXoff = -194;
    // AcYoff = -30;
    // AcZoff = -1857;
    //Temperature correction

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

    //read gyro data
    GyX = (Wire.read() << 8 | Wire.read());
    GyY = (Wire.read() << 8 | Wire.read());
    GyZ = (Wire.read() << 8 | Wire.read());


    // write to sd card(MOSI-pin 11, MISO-pin 12, CLK-pin 13, CS-pin 4)

    // Get baro data

    lps.getEvent(&pressure, &temp);// get pressure
    runs += 1;
    //send the data out the serial port
    
    
  }
  
}

void printData(void *pvParameters)
{
  (void)pvParameters;

  for (;;) {
    Serial.print("AcX:");
    Serial.print(AcX);
    Serial.print(", AcY:");
    Serial.print(AcY);
    Serial.print(", AcZ:");
    Serial.print(AcZ);
    Serial.print(", GyX:");
    Serial.print(GyX);
    Serial.print(", GyY:");
    Serial.print(GyY);
    Serial.print(", GyZ:");
    Serial.print(GyZ);
    Serial.print(", tx:");
    Serial.print(tx);
    Serial.print(", bt:");
    Serial.print(temp.temperature);
    Serial.print(", pr:");
    Serial.print(pressure.pressure);
    Serial.print(", cs:");
    Serial.println(runs/(millis()-start));
  }
}
