#ifndef __MYOS__DRIVERS__AMD__AM79C973_H
#define __MYOS__DRIVERS__AMD__AM79C973_H

#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <hardwarecommunication/port.h>

namespace myos
{
    namespace drivers
    {

        class amd_am79c973 : public Driver, public myos::hardwarecommunication::InterruptHandler
        {
        private:
            
            struct InitializationBlock
            {   
                myos::common::uint16_t mode;
                unsigned reserved1 : 4;
                unsigned numSendBuffers : 4;
                unsigned reserved2 : 4;
                unsigned numRecvBuffers : 4;
                myos::common::uint64_t physicalAddress : 48;
                myos::common::uint16_t reserved3;
                myos::common::uint64_t logicalAddress;
                myos::common::uint32_t recvBufferDescrAddress;
                myos::common::uint32_t sendBufferDescrAddress;
            } __attribute((packed));

            struct BufferDescriptor
            {
                myos::common::uint32_t address;
                myos::common::uint32_t flags;
                myos::common::uint32_t flags2;
                myos::common::uint32_t avail;
            } __attribute__((packed));

            myos::hardwarecommunication::Port16Bit MACAddress0Port;
            myos::hardwarecommunication::Port16Bit MACAddress2Port;
            myos::hardwarecommunication::Port16Bit MACAddress4Port;
            myos::hardwarecommunication::Port16Bit registerDataPort;
            myos::hardwarecommunication::Port16Bit registerAddressPort;
            myos::hardwarecommunication::Port16Bit resetPort;
            myos::hardwarecommunication::Port16Bit busControlRegisterDataPort;

            InitializationBlock initBlock;

            BufferDescriptor* sendBufferDescr;
            myos::common::uint8_t sendBufferDescMemory[2048+15];
            myos::common::uint8_t sendbuffers[2*1024+15][8];
            myos::common::uint8_t currentSendBuffer;

            BufferDescriptor* recvBufferDescr;
            myos::common::uint8_t recvBufferDescMemory[2048+15];
            myos::common::uint8_t recvbuffers[2*1024+15][8];
            myos::common::uint8_t currentRecvBuffer;

        public:
            amd_am79c973(myos::hardwarecommunication::PeripheralComponentInterconnectDeviceDescriptor* dev, myos::hardwarecommunication::InterruptManager* interrupts);
            ~amd_am79c973();

            void Activate();
            int Reset();
            myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);

            void Send(myos::common::uint8_t* buffer, int size);
            void Receive();
        };

    }
}

#endif // !__MYOS__DRIVERS__AMD__AM79C973_H