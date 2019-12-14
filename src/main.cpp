#include <Arduino.h>
#include "console.h"
#include <driver/adc.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <time.h>
#include <WiFi.h>

#define LED 2
//#define STASSID "Telecom-STV"
//#define STAPSK  "TnD71xFNi3"
#define STASSID "Dragon"
#define STAPSK  "TgN399eA"
#define CONN_ATTEMPTS 20

const char* ssid     = STASSID;
const char* password = STAPSK;
const char* host     = "esp8266sd";
long timezone = 3; 
byte daysavetime = 1;

// разбор команд
/******************************************************************/

void _test_(ArgList& L, Stream& S)
{
  String p;
  int i=0;
  while(!(p = L.getNextArg()).isEmpty())
    S.printf("arg%d = \"%s\"\n", i++, p.c_str());
  /*
  String x = "abcdefgh 0123456789";
  i = x.indexOf(' ');
  String y = x.substring(5,10);
  S.printf("%s %d %c %c\r\n", x.c_str(), i, x.charAt(i+1), x.charAt(0));
  S.println(y);
  S.printf("%d %c\r\n", (int)x[20], x[20]);
  try
  {
    S.println(x.charAt(90));
  }
  catch(const std::exception& e)
  {
    S.println(e.what());
  }
  */
}

void _adc_(ArgList& L, Stream& S)
{
  for (int i = 0; i<8; i++)
  {
    uint16_t v = adc1_get_raw(ADC1_CHANNEL_6);
    S.printf("%2d: V = %d\r\n", i, v);
    v = hall_sensor_read();
    S.printf("H = %d\r\n", v);
    float t = temperatureRead();
    S.printf("t = %0.1f°C\r\n", t);
  }
}

void _led_(ArgList& L, Stream&)
{
  String p = L.getNextArg();
  if (p == "1")
    digitalWrite(LED,HIGH);
  else
    digitalWrite(LED,LOW);
}

void _unknown(String& L, Stream& S)
{
  S.print("? ");
  S.println(L);
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file)
  {
    if(file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.print (file.name());
      time_t t= file.getLastWrite();
      struct tm * tmstruct = localtime(&t);
      Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",
        (tmstruct->tm_year)+1900,
        (tmstruct->tm_mon)+1, 
         tmstruct->tm_mday,
         tmstruct->tm_hour, 
         tmstruct->tm_min, 
         tmstruct->tm_sec
        );
      if(levels)
      {
        listDir(fs, file.name(), levels -1);
      }
    } 
    else 
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.print(file.size());
      time_t t= file.getLastWrite();
      struct tm * tmstruct = localtime(&t);
      Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",
        (tmstruct->tm_year)+1900,
        (tmstruct->tm_mon)+1, 
         tmstruct->tm_mday,
         tmstruct->tm_hour, 
         tmstruct->tm_min, 
         tmstruct->tm_sec
        );
    }
    file = root.openNextFile();
  }
}

void _list_(ArgList& L, Stream&)
{
  listDir(SD, "/", 0);
}

void writeFile(fs::FS &fs, const char * path, const char * message)
{
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message))
  {
    Serial.println("File written");
  } 
  else 
  {
    Serial.println("Write failed");
  }
  file.close();
}

void _write_(ArgList& L, Stream&)
{
  String s = L.getNextArg();
  String t = L.getNextArg();
  writeFile(SD, s.c_str(), t.c_str());
}
/******************************************************************/

Console con;

void setup() 
{
  pinMode(LED,OUTPUT);
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_11);
  Serial.begin(115200);
  while (!Serial) yield();
  Serial.println(F("------------------------------------------------test start"));
  Serial.printf("F=%dMHz Heap=%d %02X\r\n",ESP.getCpuFreqMHz(), ESP.getHeapSize(), ESP.getChipRevision());
  Serial.printf("FCS=%d FCS=%dHz FCM=%d\r\n",ESP.getFlashChipSize(), ESP.getFlashChipSpeed(), ESP.getFlashChipMode()); // 0=QIO, 1=QOUT, 2=DIO, 3=DOUT
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print(F("Connecting to "));
  Serial.print(ssid);
  
  // Wait for connection
  int i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < CONN_ATTEMPTS) 
  {
    Serial.print('.');
    delay(250);
  }
  Serial.println();
  if (i > CONN_ATTEMPTS) 
  {
    Serial.print(F("Could not connect to "));
    Serial.println(ssid);
  }
  else
  {
    Serial.print(F("Connected! IP address: "));
    Serial.println(WiFi.localIP());
  }
  
  // Time server connection
  Serial.println("Contacting Time Server");
	configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "ntp2.stratum2.ru", "ntp5.stratum2.ru");
	struct tm tmstruct;
  //delay(2000);
  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct, 10000);
	Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n",
    (tmstruct.tm_year)+1900,
    (tmstruct.tm_mon)+1, 
     tmstruct.tm_mday,tmstruct.tm_hour, 
     tmstruct.tm_min, 
     tmstruct.tm_sec
    );
  Serial.println("");
  
  // SD card init
  if(!SD.begin())
  {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC)
  {
    Serial.println("MMC");
  } 
  else if(cardType == CARD_SD)
  {
    Serial.println("SDSC");
  } 
  else if(cardType == CARD_SDHC)
  {
    Serial.println("SDHC");
  } 
  else 
  {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  // Console setup
  con.onCmd("test", _test_);
  con.onCmd("led",  _led_ );
  con.onCmd("adc",  _adc_ );
  con.onCmd("list", _list_);
  con.onCmd("wr", _write_);
//  con.onUnknown(_unknown);  
  con.onUnknown([](String& L, Stream& S){S.print("? ");S.println(L);}); // лямбда-функция !
  con.start(); 
}

void loop() 
{
  con.run();
}
