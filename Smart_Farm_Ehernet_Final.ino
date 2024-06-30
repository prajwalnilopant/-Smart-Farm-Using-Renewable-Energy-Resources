#include "dht.h"  
#include <MQ2.h>
#include <SPI.h>
#include <Ethernet.h>
#include "ThingSpeak.h"

#define dht_apin 8//8                                                                   
#define Hydrogen_sensor A3
int UVOUT = A0;                                         // 3.3V = 3.3V    OUT = A0    GND = GND      3.3V = A1      EN=3.3V
int REF_3V3 = A1;
//WaterLevel Pin A4


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient client;

unsigned long myChannelNumber = 993511;
const char * myWriteAPIKey = "1WOTAGYU99E2A7OL" ;

dht DHT;
int hum,temp,HydrogenLevel;
IPAddress ip(192, 168, 137, 1);

void setup() 
{
    Serial.begin(9600);                                                                        
    pinMode(Hydrogen_sensor, INPUT);
    pinMode(UVOUT, INPUT);
    pinMode(REF_3V3, INPUT);
    if (Ethernet.begin(mac) == 0) 
    {
      Serial.println("Failed to configure Ethernet using DHCP");                              
      Ethernet.begin(mac, ip);
    }
    delay(250);
    Serial.println("connecting...");
    ThingSpeak.begin(client);  
    delay(500);
    pinMode(2, INPUT);
}

void loop()
{
    DHT.read11(dht_apin);
    hum=DHT.humidity;
    temp=DHT.temperature;
    HydrogenLevel=digitalRead(Hydrogen_sensor);
    int uvLevel = averageAnalogRead(UVOUT);
    int refLevel = averageAnalogRead(REF_3V3);
    int value = analogRead(A4);
    float outputVoltage = 3.3 / refLevel * uvLevel;
    float uvIntensity = mapfloat(outputVoltage, 0.99, 2.9, 0.0, 15.0);
    delay(1000);                                                                          
    
    Serial.println("");
    Serial.print("Current humidity = ");
    Serial.print(hum);
    Serial.print("%  ");
    
    Serial.print("temperature = ");
    Serial.print(temp); 
    Serial.println("C  ");
    
    Serial.print("pH: ");
    Serial.print(HydrogenLevel);

    Serial.print("\nML8511 output: ");
    Serial.print(uvLevel);
    Serial.print(" nm");
    
    Serial.print(" UV Intensity (mW/cm^2): ");
    Serial.println(uvIntensity);
    
    Serial.print("Water Level:");
    Serial.println(value);
    Serial.println();
    
    
    ThingSpeak.setField(1, uvLevel);
    ThingSpeak.setField(2, value);
    ThingSpeak.setField(3, HydrogenLevel);
    ThingSpeak.setField(4, temp);
    ThingSpeak.setField(5, hum);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    delay(2000);
}



int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 
  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;
  return(runningValue);  
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
