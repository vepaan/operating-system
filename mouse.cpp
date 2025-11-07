#include "mouse.h"

MouseDriver::MouseDriver(InterruptManager* manager)
: InterruptHandler(0x2C, manager), //mouse interrupt is 0x2C
  dataport(0x60), 
  commandport(0x64)
{
  offset = 0; // between 0-2 try and error
  buttons = 0;

  uint16_t* VideoMemory = (uint16_t*)0xb8000;

  VideoMemory[80*12+40] = ((VideoMemory[80*12+40] & 0xF000) >> 4)
                      | ((VideoMemory[80*12+40] & 0x0F00) << 4)
                      | ((VideoMemory[80*12+40] & 0x0FF));

  commandport.Write(0xA8); // activate mouse interrupt
  commandport.Write(0x20); // get current state
  uint8_t status = dataport.Read() | 2;
  commandport.Write(0x60); // set state
  dataport.Write(status);

  commandport.Write(0xD4);
  dataport.Write(0xF4);
  dataport.Read();
}

MouseDriver::~MouseDriver()
{
}

void printf(const char*);

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
  uint8_t status = commandport.Read();
  if (!(status & 0x20)) // only if sixth bit of status is 1, is there data to read
    return esp;

  static int8_t x=40, y=12;

  buffer[offset] = dataport.Read();
  offset = (offset + 1) % 3;

  if (offset == 0)
  {
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                        | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                        | ((VideoMemory[80*y+x] & 0x0FF));

    x += buffer[1];
    if (x < 0) x = 0;
    if (x >= 80) x = 79;

    y -= buffer[2];
    if (y < 0) y = 0;
    if (y >= 25) y = 24;

    VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                        | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                        | ((VideoMemory[80*y+x] & 0x0FF));
  }
  
  return esp;
}
