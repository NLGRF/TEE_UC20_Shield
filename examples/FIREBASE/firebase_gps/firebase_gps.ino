#include "TEE_UC20.h"
#include "SoftwareSerial.h"
#include <AltSoftSerial.h>
#include "internet.h"
#include "firebase.h"
#include "gnss.h"
GNSS gps;

INTERNET net;
FIREBASE firebase;
//SIM TRUE  internet
#define APN "internet"
#define USER ""
#define PASS ""

#define FIREBASE_URL "Firebase URL"
#define FIREBASE_SECRET "Database Secrets"

//SoftwareSerial mySerial(8, 9); // RX, TX
AltSoftSerial mySerial;
void debug(String data)
{
  Serial.println(data);
}
void data_out(char data)
{
  Serial.write(data);
}

int count = 0;

void setup()
{
  Serial.begin(9600);
  gsm.begin(&mySerial, 9600);
  gsm.Event_debug = debug;
  Serial.println(F("UC20"));
  gsm.PowerOn();
  while (gsm.WaitReady())
  {
  }
  Serial.print(F("GetOperator --> "));
  Serial.println(gsm.GetOperator());
  Serial.print(F("SignalQuality --> "));
  Serial.println(gsm.SignalQuality());
  Serial.println(F("Disconnect net"));
  net.DisConnect();
  Serial.println(F("Set APN and Password"));
  net.Configure(APN, USER, PASS);
  Serial.println(F("Connect net"));
  net.Connect();
  Serial.println(F("Show My IP"));
  Serial.println(net.GetIP());
  firebase.begin(FIREBASE_URL, FIREBASE_SECRET);
  int ret = firebase.connect();
  if (ret == 1)
  {
    Serial.println(F("Firebase Connected"));
  }
  firebase.close();
  gps.Start();
  Serial.print(F("GPS Start"));
}

void loop()
{
  int ret = firebase.connect();
  int state = firebase.getInt("State");
  /*
     // Define
    String str = "This is my string";

    // Length (with one extra character for the null terminator)
    int str_len = str.length() + 1;

    // Prepare the character array (the buffer)
    char char_array[str_len];

    // Copy it over
    str.toCharArray(char_array, str_len);
  */
  // +QGPSLOC: <UTC>,<latitude>,<longitude>,<hdop>,<altitude>,<fix>,<cog>,<spkm>,<spkn>,<date>, <nsat>
  String UTC = "";
  String latitude = "";
  String longitude = "";

  String tl = gps.GetPosition();
  Serial.println(tl);
  int tl_len = tl.length() + 1;
  char tl_array[tl_len];
  tl.toCharArray(tl_array, tl_len);

  for (int i = 10; i < tl.length() + 1; i++)
  {
    if (tl[i] == ',')
    {
      //      Serial.println(count);
      count++;
    }
    else if (tl[i] != ',' && count == 0)
    {
      UTC += tl[i];
    }
    else if (tl[i] != ',' && count == 1)
    {
      latitude += tl[i];
    }
    else if (tl[i] != ',' && count == 2)
    {
      longitude += tl[i];
    }
  }

  Serial.println(UTC);
  Serial.println(latitude);
  Serial.println(longitude);
  if (ret == 1)
  {
    if (state == 1)
    {
      firebase.setStr("UTC", UTC);
      firebase.setStr("latitude", latitude);
      firebase.setStr("longitude", longitude);
      // firebase.setStr("Test", "1.23456");
      count = 0;
      Serial.println(F("Firebase OK"));
    }
    else
    {
      Serial.println(F("Firebase Error"));
    }
  }
  firebase.close();
  delay(2000);
}
