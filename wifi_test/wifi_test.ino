 #include <LTask.h>
  #include <LWiFi.h>
  #include <LWiFiClient.h>

  #define WIFI_AP "chidori"
  #define WIFI_PASSWORD "shaggy95"
  #define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.

  // Ubidots information

  #define URL    "things.ubidots.com"
  #define TOKEN  "Sxxxxxxxxx9m4wX"          // replace with your Ubidots token generated in your profile tab
  #define VARID1 "55fda54a762542789fe3415b"                // create a variable in Ubidots and put its ID here (http://app.ubidots.com/ubi/datasources/)


  void setup()
  {
    LTask.begin();
    LWiFi.begin();
    Serial.begin(19200);

    // keep retrying until connected to AP
    Serial.println("Connecting to AP");
    while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
    {
      delay(1000);
    }
  }

  // I plan to use this function in the future to reconnect to WiFi if the connection drops:
  boolean wifi_status(LWifiStatus ws){
    switch(ws){
      case LWIFI_STATUS_DISABLED:
        return false;
      break;
      case LWIFI_STATUS_DISCONNECTED:
        return false;
      break;
      case LWIFI_STATUS_CONNECTED:
        return true;
      break;
    }
    return false;
  }


  void loop()
  {   
    Serial.println("Connecting to Ubidots...");
    LWiFiClient c;
    while (!c.connect(URL, 80))
    {
      Serial.println("Retrying to connect...");
      delay(100);
    }

    Serial.println("Connected!");

    while(1){
      LWifiStatus ws = LWiFi.status();
      boolean status = wifi_status(ws);
      if(!status){
        Serial.println("Connecting to AP");
        while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
        {
          delay(500);
        }
      }
      String payload = "{\"value\":"+ String(analogRead(A0)) + "}";
      String le = String(payload.length());    

      if(!c.connected()){
        while (!c.connect(URL, 80)){
          delay(100);
        }
        Serial.println("Client reconnected!");
      }   

      // Build HTTP POST request
      c.print(F("POST /api/v1.6/variables/"));
//      c.print(ID);
      c.println(F("/values HTTP/1.1"));
      c.println(F("User-Agent: LinKit One/1.0"));
      c.print(F("X-Auth-Token: "));
      c.println(TOKEN);
      c.println(F("Connection: close"));
      c.println(F("Content-Type: application/json"));
      c.print(F("Content-Length: "));
      c.println(le);
      c.print(F("Host: "));
      c.println(URL);
      c.println(); 
      c.println(payload);
      c.println(); 

      int v;
      while(c.available()){  
        v = c.read();
        if(v < 0){
          Serial.println("No response.");
          break;
        }
        // Serial.print((char)v);  // Add this line to the code in test case
      }
      delay(1000);
    }
  }
