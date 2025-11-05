#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"

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
    printf("Hello World!");
    printf("Hello World!");

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);

    KeyboardDriver keyboard(&interrupts);

    interrupts.Activate();

    while(1);
}
