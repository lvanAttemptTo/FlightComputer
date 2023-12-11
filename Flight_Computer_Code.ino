#include <Wire.h>
#include <math.h>
#include <SPI.h>
#include <Adafruit_LPS2X.h>
#include <Adafruit_Sensor.h>
#include "FS.h"
#include "SPIFFS.h"
const int MPU = 0x68;




int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
double pitch, roll;
int RED_LED = 5;
int BLUE_LED = 6;
int GREEN_LED = 7;

Adafruit_LPS22 lps;



#define FORMAT_SPIFFS_IF_FAILED true

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void writeFileI(fs::FS &fs, const char * path, uint8_t *message, int len){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.write(message, len)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

int16_t readFileI(fs::FS &fs, const char * path, int pos){
  Serial.printf("Reading file: %s\r\n", path);
  uint8_t rV[2] = {0, 0};
  File file = fs.open(path);
  if(!file || file.isDirectory()){
      Serial.println("- failed to open file for reading");
      return 0;
  }

  Serial.println("- read from file:");
  if(pos > 0)
  {
    for(int i = 0; i < pos; i++)
    {
      if(file.available())
      {
        file.read();
      }
    }
  }
  for(int i = 0; i < 2; i++){
    if(file.available()){
      rV[i] = file.read();
    }
  }
  file.close();

  uint16_t bytes = ((uint16_t)rV[1] << 8) | ((uint16_t)rV[0]);


  union
  {
    uint16_t i;
    int16_t i2;
  }toInt;

  toInt.i = bytes;

  return toInt.i2;
}

float readFileF(fs::FS &fs, const char * path, int pos){
  Serial.printf("Reading file: %s\r\n", path);
  uint8_t rV[4];
  File file = fs.open(path);
  if(!file || file.isDirectory()){
      Serial.println("- failed to open file for reading");
      return 0;
  }

  Serial.println("- read from file:");
  if(pos > 0)
  {
    for(int i = 0; i < pos; i++)
    {
      if(file.available())
      {
        file.read();
      }
    }
  }
  for(int i = 0; i < 4; i++){
    if(file.available()){
      rV[i] = file.read();
    }
  }
  file.close();

  uint32_t bytes = ((uint32_t)rV[3] << 24) | ((uint32_t)rV[2] << 16) | ((uint32_t)rV[1] << 8) | ((uint32_t)rV[0]);
  union {
    uint32_t b;
    float f;
  } toFloat;
  toFloat.b = bytes;

  return toFloat.f;
}

unsigned long readFileL(fs::FS &fs, const char * path, int pos){
  Serial.printf("Reading file: %s\r\n", path);
  uint8_t rV[4];
  File file = fs.open(path);
  if(!file || file.isDirectory()){
      Serial.println("- failed to open file for reading");
      return 0;
  }

  Serial.println("- read from file:");
  if(pos > 0)
  {
    for(int i = 0; i < pos; i++)
    {
      if(file.available())
      {
        file.read();
      }
    }
  }
  for(int i = 0; i < 4; i++){
    if(file.available()){
      rV[i] = file.read();
    }
  }
  file.close();

  uint32_t bytes = ((uint32_t)rV[3] << 24) | ((uint32_t)rV[2] << 16) | ((uint32_t)rV[1] << 8) | ((uint32_t)rV[0]);
  union {
    uint32_t b;
    unsigned long l;
  } toLong;
  toLong.b = bytes;

  return toLong.l;
}

void appendFileI(fs::FS &fs, const char * path, uint8_t *message, int len){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.write(message, len)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}



void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}


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
  while (!Serial) delay(10);

  if (!lps.begin_I2C())
  {
      Serial.println("No baromenter");
      while (1) delay(10);
  }

  lps.setDataRate(LPS22_RATE_75_HZ);
  Serial.begin(115200);
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed");
      return;
  }
  deleteFile(SPIFFS, "/test.bin");

  for (int i = 0; i < 3; i++)
  {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 14, true);
    int tempAcc;
    int16_t tx; 
    
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
    //get pitch/roll


    

    // Get baro data
    sensors_event_t temp;
    sensors_event_t pressure;
    lps.getEvent(&pressure, &temp);// get pressure


    unsigned long tm = millis();

    //send the data out the serial port
    

    // write all the data to the nand flash

    uint8_t *bin;
    bin = (uint8_t*)(&AcX);
    appendFileI(SPIFFS, "/test.bin", bin, 2);

    uint8_t *bin0;
    bin0 = (uint8_t*)(&AcY);
    appendFileI(SPIFFS, "/test.bin", bin0, 2);

    uint8_t *bin1;
    bin1 = (uint8_t*)(&AcZ);
    appendFileI(SPIFFS, "/test.bin", bin1, 2);

    uint8_t *bin2;
    bin2 = (uint8_t*)(&GyX);
    appendFileI(SPIFFS, "/test.bin", bin2, 2);

    uint8_t *bin3;
    bin3 = (uint8_t*)(&GyY);
    appendFileI(SPIFFS, "/test.bin", bin3, 2);

    uint8_t *bin4;
    bin4 = (uint8_t*)(&GyZ);
    appendFileI(SPIFFS, "/test.bin", bin4, 2);

    uint8_t *bin5;
    bin5 = (uint8_t*)(&tx);
    appendFileI(SPIFFS, "/test.bin", bin5, 2);

    uint8_t *bin6;
    bin6 = (uint8_t*)(&temp.temperature);
    appendFileI(SPIFFS, "/test.bin", bin6, 4);
    
    uint8_t *bin7;
    bin7 = (uint8_t*)(&pressure.pressure);
    appendFileI(SPIFFS, "/test.bin", bin7, 4);

    uint8_t *bin8;
    bin8 = (uint8_t*)(&tm);
    appendFileI(SPIFFS, "/test.bin", bin8, 4);



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
    Serial.print(pressure.pressure);
    Serial.print(" ");
    Serial.println(tm);

    delay(250);
  }

  listDir(SPIFFS, "/", 0);

  File file = SPIFFS.open("/test.bin");

  int fileSize = file.size();

  for(int i = 0; i < fileSize; i += 26)
  {
    Serial.println(readFileI(SPIFFS, "/test.bin", i)); // AcX
    Serial.println(readFileI(SPIFFS, "/test.bin", i+2)); // AcY
    Serial.println(readFileI(SPIFFS, "/test.bin", i+4)); // AcZ
    Serial.println(readFileI(SPIFFS, "/test.bin", i+6)); // GyX
    Serial.println(readFileI(SPIFFS, "/test.bin", i+8)); // GyY
    Serial.println(readFileI(SPIFFS, "/test.bin", i+10)); // GyZ
    Serial.println(readFileI(SPIFFS, "/test.bin", i+12)); // tx
    Serial.println(readFileF(SPIFFS, "/test.bin", i+14)); // temp.temperature
    Serial.println(readFileF(SPIFFS, "/test.bin", i+18)); // pressure.pressure
    Serial.println(readFileL(SPIFFS, "/test.bin", i+22)); // tm
    
  }

  deleteFile(SPIFFS, "/test.bin");
}

void loop()
{

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
