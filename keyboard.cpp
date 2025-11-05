#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(0x21, manager), //keyboard interrupt is 0x21
  dataport(0x60), 
  commandport(0x64)
{
  // to handle when we boot os with a key pressed. wait until that key is released
  while(commandport.Read() & 0x1)
    dataport.Read();

  commandport.Write(0xAE); // activate interrupts for keyboard CPU
  commandport.Write(0x20); // get current state
  uint8_t status = (dataport.Read() | 1) & ~0x10; // clear fifth bit
  commandport.Write(0x60); // set state
  dataport.Write(status);

  dataport.Write(0xF4);
}

KeyboardDriver::~KeyboardDriver()
{
}

void printf(const char*);

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
  uint8_t key = dataport.Read();

  char* foo = "KEYBOARD 0X00";
  char* hex = "0123456789ABCDEF";
  foo[11] = hex[(key >> 4) & 0x0F];
  foo[12] = hex[key & 0x0F];
  printf(foo);

  return esp;
}
