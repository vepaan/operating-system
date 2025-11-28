#ifndef __MYOS__DRIVERS__ATA_H
#define __MYOS__DRIVERS__ATA_H

#include <hardwarecommunication/port.h>

namespace myos
{
    namespace drivers
    {
        class AdvancedTechnologyAttachment
        {
        protected:
            myos::hardwarecommunication::Port16Bit dataPort;
            myos::hardwarecommunication::Port8Bit errorPort;
            myos::hardwarecommunication::Port8Bit sectorCountPort;
            myos::hardwarecommunication::Port8Bit lbaLowPort;
            myos::hardwarecommunication::Port8Bit lbaMidPort;
            myos::hardwarecommunication::Port8Bit lbaHiPort;
            myos::hardwarecommunication::Port8Bit devicePort;
            myos::hardwarecommunication::Port8Bit commandPort;
            myos::hardwarecommunication::Port8Bit controlPort;
        };
    }
}

#endif // !__MYOS__DRIVERS__ATA_H