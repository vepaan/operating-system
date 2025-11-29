#include <syscalls.h>

using namespace myos;
using namespace myos::common;
using namespace myos::hardwarecommunication;

void printf(const char*);

SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t interruptNumber)
:   InterruptHandler(interruptNumber + interruptManager->HardwareInterruptOffset(), interruptManager)
{
}

SyscallHandler::~SyscallHandler()
{
}

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp)
{
    CPUState* cpu = (CPUState*)esp;

    switch(cpu->eax)
    {
        case 4:
            printf((const char*)cpu->ebx);
            break;

        default:
            break;
    }

    return esp;
}
