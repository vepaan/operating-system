#ifndef __MYOS__NET__ETHERFRAME_H
#define __MYOS__NET__ETHERFRAME_H

#include <common/types.h>
#include <drivers/amd_am79c973.h>
#include <memorymanagement.h>

namespace myos
{
  namespace net
  {
    struct EtherFrameHeader
    {
      myos::common::uint64_t dstMAC_BE : 48;
      myos::common::uint64_t srcMAC_BE : 48;
      myos::common::uint16_t etherType_BE;
    } __attribute__((packed));

    typedef myos::common::uint32_t EtherFrameFooter;

    class EtherFrameProvider;

    class EtherFrameHandler
    {
    protected:
      EtherFrameProvider* backend;
      myos::common::uint16_t etherType_BE;

    public:
      EtherFrameHandler(EtherFrameProvider* backend, myos::common::uint16_t etherType_BE);
      ~EtherFrameHandler();

      virtual bool OnEtherFrameReceived(myos::common::uint8_t* etherframePayload, myos::common::uint32_t size);
      void Send(myos::common::uint64_t dstMAC_BE, myos::common::uint8_t* etherframePayload, myos::common::uint32_t size);

    };

    class EtherFrameProvider : public myos::drivers::RawDataHandler
    {
    friend class EtherFrameHandler;

    protected:
      EtherFrameHandler* handlers[65535];

    public:
      EtherFrameProvider(myos::drivers::amd_am79c973* backend);
      ~EtherFrameProvider();
      
      virtual bool OnRawDataReceived(myos::common::uint8_t* buffer, myos::common::uint32_t size);
      virtual void Send(myos::common::uint64_t dstMAC_BE, myos::common::uint16_t etherType_BE, myos::common::uint8_t* buffer, myos::common::uint32_t size);
      
      myos::common::uint64_t GetMACAddress();
      myos::common::uint32_t GetIPAddress();
    };
  }
}

#endif // !__MYOS__NET__ETHERFRAME_H
