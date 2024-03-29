#include "TEE_UC20.h"
#include "SoftwareSerial.h"
#include <AltSoftSerial.h>
#include "internet.h"
#include "firebase.h"

INTERNET net;
FIREBASE firebase;
//SIM TRUE  internet
#define APN "internet"
#define USER ""
#define PASS ""

#define LED 10
#define FIREBASE_URL      "Firebase URL"
#define FIREBASE_SECRET   "Database Secrets"
unsigned long previousMillis = 0;
const long interval = 2000; 

AltSoftSerial mySerial;

void debug(String data)
{
  Serial.println(data);
}
void setup() 
{
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);
  Serial.begin(9600);
  gsm.begin(&mySerial,9600);
  gsm.Event_debug = debug;
  Serial.println(F("UC20"));
  gsm.PowerOn(); 
  while(gsm.WaitReady()){}
  Serial.print(F("GetOperator --> "));
  Serial.println(gsm.GetOperator());
  Serial.print(F("SignalQuality --> "));
  Serial.println(gsm.SignalQuality());
  Serial.println(F("Disconnect net"));
  net.DisConnect();
  Serial.println(F("Set APN and Password"));
  net.Configure(APN,USER,PASS);
  Serial.println(F("Connect net"));
  net.Connect();
  Serial.println(F("Show My IP"));
  Serial.println(net.GetIP());
  firebase.begin(FIREBASE_URL,FIREBASE_SECRET);
  int ret = firebase.connect();
  if(ret==1)
  { 
      Serial.println("Remove Object LED");
      firebase.remove("LED");
      Serial.println("Create Object LED");
      firebase.setInt("LED",0);
  }
   firebase.close();
   previousMillis =  millis();
}
void loop() 
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) 
  {
   int ret = firebase.connect();
   if(ret==1)
    { 
      int r = random(1,10);
      firebase.setInt("Random",r);
      int led = firebase.getInt("LED");
      Serial.println("Get LED = "+String(led));
      if(led==1)
              digitalWrite(LED,LOW);
      else
              digitalWrite(LED,HIGH);

    }
    firebase.close();
    previousMillis = currentMillis;
  }
}
