#include <LGPRS.h>
#include <LGPRSClient.h>
#include <LGPRSServer.h>

#include <LGPS.h>
#include <HttpClient.h>
#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>
#include <LDateTime.h>
  #define WIFI_AP "chidori"
  #define WIFI_PASSWORD "shaggy95"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
#define per 50
#define per1 3
#define DEVICEID "DEHGaKt6"
#define DEVICEKEY "D6HN8LXwej3VOsbd"
#define SITE_URL "api.mediatek.com"

gpsSentenceInfoStruct info;
char buff[256];
double latitude;
double longitude;

char buffer_latitude[8];
char buffer_longitude[8];
LGPRSClient c;
//LWiFiClient c;
unsigned int rtc;
unsigned int lrtc;
unsigned int rtc1;
unsigned int lrtc1;
char port[4]={0};
char connection_info[21]={0};
char ip[15]={0};             
int portnum;
int val = 0;
String tcpdata = String(DEVICEID) + "," + String(DEVICEKEY) + ",0";
String tcpcmd_led_on = "LED_CONTROL,1";
String tcpcmd_led_off = "LED_CONTROL,0";
String upload_led;

LGPRSClient c2;
//LWiFiClient c2;
HttpClient http(c2);

void setup()
{
    Serial.begin(115200);
  pinMode(13, OUTPUT);
  Serial.println("Task Begin");
  LTask.begin();
  Serial.println("LWifi Begin");
  LWiFi.begin();

  LGPS.powerOn();
  //while(!Serial);
  
  AP_connect();
  getconnectInfo();
  connectTCP();
  Serial.println("==============================");
}

void loop()
{
  String tcpcmd="";
  while (c.available())
   {
      int v = c.read();
      if (v != -1)
      {
        Serial.print((char)v);
        tcpcmd += (char)v;
        if (tcpcmd.substring(40).equals(tcpcmd_led_on)){
          digitalWrite(13, HIGH);
          Serial.print("Switch LED ON ");
          tcpcmd="";
        }else if(tcpcmd.substring(40).equals(tcpcmd_led_off)){  
          digitalWrite(13, LOW);
          Serial.print("Switch LED OFF");
          tcpcmd="";
        }
      }
   }

  LDateTime.getRtc(&rtc);
  if ((rtc - lrtc) >= per) {
    heartBeat();
    lrtc = rtc;
  }
  //Check for report datapoint status interval
  LDateTime.getRtc(&rtc1);
  if ((rtc1 - lrtc1) >= per1) {
    
    uploadstatus();
    GPS_receive();
    uploadGPS();
    lrtc1 = rtc1;
  }
}
