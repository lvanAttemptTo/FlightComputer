#include <SoftwareSerial.h>

// The serial connection to the GPS module
SoftwareSerial ss(3, 2);
char sentenceCode[7];
int sentenceCodePointer = 0;

void setup(){
  Serial.begin(9600);
  ss.begin(9600);
}

void loop(){
  while (ss.available() > 0){
    // get the byte data from the GPS
    byte gpsData = ss.read();
    // Serial.write(gpsData);
    if(gpsData == 36)
    {
      strcpy(sentenceCode, "");
      sentenceCodePointer = 0;
    }
    if(sentenceCodePointer < 6)
    {
      sentenceCode[sentenceCodePointer] = gpsData;
      sentenceCodePointer += 1;
    }
    if(sentenceCodePointer == 6)
    {
      sentenceCode[6] = '\0';
      Serial.println(sentenceCode);
      sentenceCodePointer = 7;
    }
  }
}