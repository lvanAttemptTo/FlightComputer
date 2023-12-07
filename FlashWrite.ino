#include "FS.h"
#include "SPIFFS.h"

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true
uint8_t readVal[4];

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
void readFileI(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    for(int i = 0; i < file.size(); i++){
      if(file.available()){
       readVal[i] = file.read();
      }
    }
    file.close();
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
  for(int i = 0; i < file.size(); i++){
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

void testFileIO(fs::FS &fs, const char * path){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    Serial.print("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          Serial.print(".");
        }
        file.write(buf, 512);
    }
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              Serial.print(".");
            }
            len -= toRead;
        }
        Serial.println("");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("- failed to open file for reading");
    }
}

void setup(){
  Serial.begin(115200);
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed");
      return;
  }


  deleteFile(SPIFFS, "/test.bin");
  float fVals[100];
  for(int i = 0; i < 100; i++)
  {
    fVals[i] = (float)(random(-10000000, 10000000)/random(-10000000, 10000000));
    float f = fVals[i];
    uint8_t *bin;
    bin = (uint8_t*)(&f);
    // uint8_t seperator = 0;  
    appendFileI(SPIFFS, "/test.bin", bin, 4);
  }

  delay(5000);
  


  listDir(SPIFFS, "/", 0);

  int fSize = 400;


  bool corr = true;
  for(int i = 0; i < fSize; i += 4)
  {
    float fVal = readFileF(SPIFFS, "/test.bin", i);
    if (fVals[i] != fVal)
    {
      corr = false;
    }
  }
  Serial.println(corr);

  
  

  
  deleteFile(SPIFFS, "/test.bin");
  
}

void loop(){

}
