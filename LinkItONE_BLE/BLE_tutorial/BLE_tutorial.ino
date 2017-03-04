#include "GATTSUart.h"
#include <LGATTUUID.h>
#include <LBTServer.h>

#ifdef APP_LOG
#undef APP_LOG
#endif
#define APP_LOG(...) Serial.printf(__VA_ARGS__); \
  Serial.println();

GATTSUart uart; //This is the UART profile for the BLE. It's not an standard one

void setup() {
  //Serial initialization
  Serial.begin(9600);
  APP_LOG("BLE tutorial starts");

  //BT initialization
  //Tip: for changing device name, we must initialize LinkIt Bluetooth, change the name,
  //close bluetooth, and start with the GATT initialization.
  const char *deviceName = "GATT_EXAMPLE";
  bool success = LBTServer.begin((uint8_t*) deviceName);
  delay(500);
  if (success) {
    APP_LOG("BT config sucessfully");
    //Disconnecting BT for opening BLE.
    LBTServer.end();
    //LinkIt will work as peripheral, so it assumes the role of GATT Server.
    //The GATT server for this example is initialized with only one profile.
    if (!LGATTServer.begin(1, &uart)) {
      APP_LOG("[FAILED] GATTS begin");
    }
    else {
      APP_LOG("GATTS begin");
    }
  }
  else {
    LBTServer.end();
    Serial.println("[FAILED] BT config successfully");
  }  
}
 
void loop() {
  delay(5000);
  LGATTServer.handleEvents(); //Initialization of the GATT handle.
  //This sketch will send a message to master each 5 seconds.
  if (uart.isConnected()) {
    const char *message = "Hello master";
    bool msgSuccess = uart.sendMessage(message); //UART function for sending messages.
    if (!msgSuccess) {
        APP_LOG("[FAILED] send [%d]", msgSuccess);
    } 
    else {
      APP_LOG("send [%d][%s]", msgSuccess, message);
    }
  }
}


