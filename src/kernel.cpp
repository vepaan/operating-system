#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <multitasking.h>
#include <syscalls.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/amd_am79c973.h>
#include <drivers/ata.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <net/etherframe.h>

//#define GRAPHICSMODE

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;
using namespace myos::gui;
using namespace myos::net;

void printf(const char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000; // Video memory address
   
    static uint8_t x=0, y=0; // dos programming screen res 80x25

    for (int i=0; str[i] != '\0'; ++i) 
    {

        switch(str[i])
        {
          case '\n':
            ++y;
            x=0;
            break;
          default:
            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i]; // to not overwrite color bit
            ++x;
            break;
        }

        if (x >= 80)
        {
          ++y;
          x=0;
        }

        // if reach bottom, clear screen and start from top (TEMP FIX)
        if (y >= 25)
        {
          for (y=0; y<25; ++y)
          {
            for (x=0; x<80; ++x)
            {
              VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            }
          }
          x=0;
          y=0;
        }
    }
}

void printfHex(uint8_t key)
{
  char* foo = "00";
  char* hex = "0123456789ABCDEF";
  foo[0] = hex[(key >> 4) & 0x0F];
  foo[1] = hex[key & 0x0F];
  printf(foo);
}

void sysprintf(const char* str)
{
  asm("int $0x80" : : "a" (4), "b" (str));
}
void taskA() { while (true) printf("A"); }
void taskB() { while (true) printf("B"); }

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors()
{
  for (constructor* i = &start_ctors; i != &end_ctors; ++i)
  {
    (*i)(); // just invoke constructors of our global objects
  }
}

extern "C" void kernelMain(void* multiboot_structure, uint32_t magicnumber)
{
    printf("Hello World!\n");

    printf("Initializing Global Descriptor Table...\n");
    GlobalDescriptorTable gdt;

    printf("Initializing Dynamic Memory Manager...\n");
    size_t heap = 10*1024*1024;
    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8); // copied from multiboot_structure official website
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);

    printf("Heap: 0x");
    printfHex((heap >> 24) & 0xFF);
    printfHex((heap >> 16) & 0xFF);
    printfHex((heap >> 8) & 0xFF);
    printfHex((heap) & 0xFF);
    void* allocated = memoryManager.malloc(1024);

    printf("\nAllocated: 0x");
    printfHex(((size_t)allocated >> 24) & 0xFF);
    printfHex(((size_t)allocated >> 16) & 0xFF);
    printfHex(((size_t)allocated >> 8) & 0xFF);
    printfHex((size_t)allocated & 0xFF);
    printf("\n");

    printf("Initializing Task Manager...\n");
    TaskManager taskManager;
    /*
    Task task1(&gdt, taskA);
    Task task2(&gdt, taskB);
    taskManager.AddTask(&task1);
    taskManager.AddTask(&task2);
    */

    printf("Initializing Interrupt Manager...\n");
    InterruptManager interrupts(0x20, &gdt, &taskManager);

    printf("Initializing System Call Handler...\n");
    SyscallHandler syscalls(&interrupts, 0x80);

    #ifdef GRAPHICSMODE
      printf("Initializing Desktop...\n");
      Desktop desktop(320, 300, 0x00, 0x00, 0xA8);
    #endif

    printf("Initializing Driver Manager...\n");
    DriverManager drvManager;

      #ifdef GRAPHICSMODE
        MouseDriver mouse(&interrupts, &desktop);
      #else
        MouseToConsole mhandler;
        MouseDriver mouse(&interrupts, &mhandler);
      #endif
      drvManager.AddDriver(&mouse);

      #ifdef GRAPHICSMODE
        KeyboardDriver keyboard(&interrupts, &desktop);
      #else
        PrintfKeyboardEventHanlder kbhandler;
        KeyboardDriver keyboard(&interrupts, &kbhandler);
      #endif
      drvManager.AddDriver(&keyboard);

      PeripheralComponentInterconnectController PCIController;
      PCIController.SelectDrivers(&drvManager, &interrupts);

      printf("Initializing VGA...\n");
      VideoGraphicsArray vga;

    printf("Activating all drivers...\n");
    drvManager.ActivateAll();

    #ifdef GRAPHICSMODE
      vga.SetMode(320, 200, 8);

      Window win1(&desktop, 10, 10, 20, 20, 0xA8, 0x00, 0x00);
      desktop.AddChild(&win1);
      Window win2(&desktop, 40, 15, 30, 30, 0x00, 0xA8, 0x00);
      desktop.AddChild(&win2);
    #endif

    AdvancedTechnologyAttachment ata0m(0x1F0, true);
    printf("ATA Primary Master: ");
    ata0m.Identify();

    AdvancedTechnologyAttachment ata0s(0x1F0, false);
    printf("ATA Primary Slave: ");
    ata0s.Identify();

    char atabuffer[] = "nahmanyoutweaking";

    ata0m.Write28(0, (uint8_t*)atabuffer, 17);
    ata0m.Flush();
    ata0m.Read28(0, (uint8_t*)atabuffer, 17);

    AdvancedTechnologyAttachment ata1m(0x170, true);
    AdvancedTechnologyAttachment ata1s(0x170, false);

    
    amd_am79c973* eth0 = (amd_am79c973*)(drvManager.drivers[2]);
    EtherFrameProvider etherframe(eth0);
    etherframe.Send(0xFFFFFFFFFFFF, 0x0608, (uint8_t*)"FOO", 3);

    //eth0->Send((uint8_t*)"Hello Network", 13);

    printf("Activating Interrupts...\n");
    interrupts.Activate();

    while(1)
    {
      #ifdef GRAPHICSMODE
        desktop.Draw(&vga);
      #endif
    }
}
