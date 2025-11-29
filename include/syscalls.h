#ifndef __MYOS__SYSCALLS_H
#define __MYOS__SYSCALLS_H

#include <hardwarecommunication/interrupts.h>
#include <common/types.h>
#include <multitasking.h>

namespace myos
{
    class SyscallHandler : public myos::hardwarecommunication::InterruptHandler
    {
    public:
        SyscallHandler(myos::hardwarecommunication::InterruptManager* interruptManager, myos::common::uint8_t interruptNumber);
        ~SyscallHandler();

        virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
    };
}

#endif // !__MYOS__SYSCALLS_H