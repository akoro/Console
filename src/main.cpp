#include <Arduino.h>
#include "console.h"
#include <driver/adc.h>

#define LED 2

// разбор команд
/******************************************************************/

void _test_(ArgList& L, Stream& S)
{
  String p;
  int i=0;
  while(!(p = L.getNextArg()).isEmpty())
    S.printf("arg%d = \"%s\"\n", i++, p.c_str());
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

/******************************************************************/

Console con;

void setup() 
{
  pinMode(LED,OUTPUT);
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_11);
  Serial.begin(115200);
  while (!Serial) yield();
  Serial.println(F("--- test console start"));
  Serial.printf("F=%dMHz Heap=%d %02X\r\n",ESP.getCpuFreqMHz(), ESP.getHeapSize(), ESP.getChipRevision());
  Serial.printf("FCS=%d FCS=%dHz FCM=%d\r\n",ESP.getFlashChipSize(), ESP.getFlashChipSpeed(), ESP.getFlashChipMode()); // 0=QIO, 1=QOUT, 2=DIO, 3=DOUT
  
  // Console setup
  con.onCmd("test", _test_);
  con.onCmd("led",  _led_ );
  con.onCmd("adc",  _adc_ );
//  con.onUnknown(_unknown);  
  con.onUnknown([](String& L, Stream& S){S.print("? ");S.println(L);}); // лямбда-функция !
  con.start(); 
}

void loop() 
{
  con.run();
}
