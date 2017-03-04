#include <LWiFi.h>
#include <LWiFiServer.h>
#include <LWiFiClient.h>


#define WIFI_AP "Robot"
#define WIFI_PASSWORD ")_+OL>p;/"
#define WIFI_AUTH LWIFI_WPA 

LWiFiServer server(80);
String readString;

const int D1 = 4, D2 = 7;
const int E1 = 5, E2 = 6;

void stop  () {digitalWrite (E1, HIGH); digitalWrite (E2, HIGH);}
void start () {digitalWrite (E1, LOW ); digitalWrite (E2, LOW );}

void setup () 
  {
  LWiFi.begin ();
  Serial.begin(9600);
  
  pinMode (D1, OUTPUT);
  pinMode (D2, OUTPUT);
  
  pinMode (E1, OUTPUT);
  pinMode (E2, OUTPUT);

  Serial.println ("Connecting to AP");
  LWiFi.connectWPA (WIFI_AP, WIFI_PASSWORD);
  printWifiStatus ();
  
  Serial.println ("Start Server");
  server.begin ();
  Serial.println ("Server Started");
  }

void loop () 
  {
  delay(500);

  LWiFiClient client = server.available();
  if (client)
    {
    while (client.connected ()) 
      {  
      if (client.available ()) 
        {
        char c = client.read ();
        if (readString.length () < 100) readString += c;
        if (c == '\n') 
          {          
          client.println ("HTTP/1.1 200 OK");
          client.println ("Content-Type: text/html");
          client.println ("Connection: close"); 
          client.println ();  
          client.println ("<!DOCTYPE HTML>");   
          client.println ("<html>");
          client.println ("<head>");
          client.println ("<title>LinkIt ONE Robot Controller</title>");
          client.println ("</head>");
          client.println ("<body>");
          client.println ("<h1>LinkIt ONE WiFi Robot Controller</h1>");
          client.println ("<br/><table><tr><td></td>");
          client.println ("<td><a href=\"/?fwd\"\"><button>^</button></a></td>");  
          client.println ("<td></td></tr><tr>");
          client.println ("<td><a href=\"/?left\"\"><button><</button></a></td>");  
          client.println ("<td><a href=\"/?back\"\"><button> &nbsp;R&nbsp;</button></a></td>");
          client.println ("<td><a href=\"/?right\"\"><button>></button></a></td>");
          client.println ("</tr><tr><td></td><td>");  
          client.println ("<a href=\"/?stop\"\"><button>&nbsp;S&nbsp;</button></a>");     
          client.println ("</td><td></td></table>");
          client.println ("<br/>"); 
          client.println ("</body>");
          client.println ("</html>");
          
          delay (5);
          client.stop ();
          
          if (readString.indexOf ("?left" ) > 0) move ('L');
          if (readString.indexOf ("?right") > 0) move ('R');
          if (readString.indexOf ("?fwd"  ) > 0) move ('F');
          if (readString.indexOf ("?back" ) > 0) move ('B');
          if (readString.indexOf ("?stop" ) > 0) stop ()   ;
          
          readString = "";
          }
        }
      }
    }
  }

void move (char direction)
  {
  start ();

  if      (direction == 'F') {digitalWrite (D1, LOW ); digitalWrite (D2, LOW );}
  else if (direction == 'B') {digitalWrite (D1, HIGH); digitalWrite (D2, HIGH);}
  else if (direction == 'L') {digitalWrite (D1, LOW ); digitalWrite (D2, HIGH);}
  else if (direction == 'R') {digitalWrite (D1, HIGH); digitalWrite (D2, LOW );}
  }

void printWifiStatus ()
  {
  Serial.print ("SSID: ");
  Serial.println (LWiFi.SSID ());

  IPAddress ip = LWiFi.localIP ();
  Serial.print ("IP Address: ");
  Serial.println (ip);

  Serial.print ("Subnet mask: ");
  Serial.println (LWiFi.subnetMask());

  Serial.print ("Gateway IP: ");
  Serial.println (LWiFi.gatewayIP());

  long rssi = LWiFi.RSSI ();
  Serial.print ("Signal strength (RSSI):");
  Serial.print (rssi);
  Serial.println (" dBm");
  }



