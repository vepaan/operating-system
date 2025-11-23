#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#include <gui/window.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;
using namespace myos::gui;

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

    printf("Initializing Interrupt Manager...\n");
    InterruptManager interrupts(&gdt);

    printf("Initializing Desktop...\n");
    Desktop desktop(320, 300, 0x00, 0x00, 0xA8);

    printf("Initializing Driver Manager...\n");
    DriverManager drvManager;

      //MouseToConsole mhandler;
      //MouseDriver mouse(&interrupts, &mhandler);
      MouseDriver mouse(&interrupts, &desktop);
      drvManager.AddDriver(&mouse);

      //PrintfKeyboardEventHanlder kbhandler;
      //KeyboardDriver keyboard(&interrupts, &kbhandler);
      KeyboardDriver keyboard(&interrupts, &desktop);
      drvManager.AddDriver(&keyboard);

      PeripheralComponentInterconnectController PCIController;
      PCIController.SelectDrivers(&drvManager, &interrupts);

      printf("Initializing VGA...\n");
      VideoGraphicsArray vga;

    printf("Activating all drivers...\n");
    drvManager.ActivateAll();

    vga.SetMode(320, 200, 8);

    Window win1(&desktop, 10, 10, 20, 20, 0xA8, 0x00, 0x00);
    desktop.AddChild(&win1);
    Window win2(&desktop, 40, 15, 30, 30, 0x00, 0xA8, 0x00);
    desktop.AddChild(&win2);

    printf("Activating Interrupts...\n");
    interrupts.Activate();

    while(1)
    {
      desktop.Draw(&vga);
    }
}
