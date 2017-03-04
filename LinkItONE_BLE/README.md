# LinkIt ONE BLE tutorial

Bluetooth Low Energy example utilizes the LinkIt ONE development board’s GATT features to manage BLE connections.

### Version
1.0.1

### Before you start

Full details on downloading and installing the Arduino IDE and LinkIt ONE SDK then configuring the IDE and upgrading the board firmware are provided in the LinkIt ONE [quick start guide].

### Understanding GATT

BLE GATT is defined by three different statements:

* Profiles: GATT Profile specifies the structure in which profile data is exchanged.
* Services: collection of data and associated behaviors to accomplish a particular function or feature of a device or portions of a device.
* Characteristics: value used in a service along with properties and configuration information about how the value is accessed and information about how the value is displayed or represented.

![alt text][gatt]

### LinkIt ONE Hardware

For the BLE tutorial we must include to the LinkIt ONE board:
* Bluetooth antenna.
* Battery for powering.

### LinkIt ONE Arduino sketch

##### Including libraries

Sketch needs thw followings libraries for working:

* LGATTUUID.h 
* LBTServer.h

##### Sending messages using BLE

After creating the GATT profile, and declaring service and characteristics, for sending messages to master through BLE can be done using the function: **sendMessage(const char ∗str)**
```c++
setup(){
    ...
    GATTSUart uart;
    LGATTServer.begin(1, &uart);
    ...
}

loop(){
    ...
    LGATTServer.handleEvents();
    const char ∗message = ”Hello master”;
    bool msgSuccess = uart.sendMessage(message);
    ...
}
```

[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. There is no need to format nicely because it shouldn't be seen. Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)


   [quick start guide]: <http://labs.mediatek.com/site/global/developer_tools/mediatek_linkit/get-started/index.gsp>
   [gatt]: https://github.com/alfonsocamberolorenzo/LinkItONE_BLE/blob/master/gatt_scheme.jpg "GATT structure"


