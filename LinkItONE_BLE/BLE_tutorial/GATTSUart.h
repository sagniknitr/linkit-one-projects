#ifndef __GATT_UART_H__
#define __GATT_UART_H__

#include "LGATTServer.h"


class GATTSUart : public LGATTService {
  public:
      uint16_t getHandle(int32_t type);
      boolean isConnected(){ return _connected; };
      uint16_t getHandleNotify(){ return _handle_notify; };
      boolean sendMessage(const char *str);
  protected:
      // prepare the data for profile
      virtual LGATTServiceInfo *onLoadService(int32_t index);
      // characteristic added
      virtual boolean onCharacteristicAdded(LGATTAttributeData &data);
      // connected or disconnected
      virtual boolean onConnection(const LGATTAddress &addr, boolean connected);
      // read action comming from master
      virtual boolean onRead(LGATTReadRequest &data);
      // write action comming from master
      virtual boolean onWrite(LGATTWriteRequest &data);
  private:
      enum TypeMessage {Life_Info, Device_Info};
      uint16_t _handle_notify;
      uint16_t _handle_write;
      boolean _connected;
};
#endif
