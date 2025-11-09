#include "mouse.h"

void printf(const char*);

// MOUSE EVENT HANDLER (ABSTRACT)
MouseEventHandler::MouseEventHandler()
{
}

void MouseEventHandler::OnActivate()
{
}

void MouseEventHandler::OnMouseDown(uint8_t button)
{
}

void MouseEventHandler::OnMouseUp(uint8_t button)
{
}

void MouseEventHandler::OnMouseMove(int x, int y)
{
}


// MOUSE CONSOLE HANDLER (CONCRETE)
MouseToConsole::MouseToConsole()
{
  x = 40;
  y = 12;

  uint16_t* VideoMemory = (uint16_t*)0xb8000;

  VideoMemory[80*12+40] = ((VideoMemory[80*12+40] & 0xF000) >> 4)
                      | ((VideoMemory[80*12+40] & 0x0F00) << 4)
                      | ((VideoMemory[80*12+40] & 0x00FF));
}

void MouseToConsole::OnMouseMove(int xoffset, int yoffset)
{
  static uint16_t* VideoMemory = (uint16_t*)0xb8000;

  VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                      | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                      | ((VideoMemory[80*y+x] & 0x00FF));

  x += xoffset;
  if (x < 0) x = 0;
  if (x >= 80) x = 79;

  y += yoffset;
  if (y < 0) y = 0;
  if (y >= 25) y = 24;

  VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                      | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                      | ((VideoMemory[80*y+x] & 0x00FF));
}


// MOUSE DRIVER
MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
: InterruptHandler(0x2C, manager), //mouse interrupt is 0x2C
  dataport(0x60), 
  commandport(0x64)
{
  this->handler = handler;
}

MouseDriver::~MouseDriver()
{
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
  uint8_t status = commandport.Read();
  if ((!(status & 0x20)) || handler == nullptr) // only if sixth bit of status is 1, is there data to read
    return esp;

  buffer[offset] = dataport.Read();
  offset = (offset + 1) % 3;

  if (offset == 0)
  {
    if (buffer[1] != 0 || buffer[2] != 0)
    {
      handler->OnMouseMove(buffer[1], -buffer[2]);
      
      
      for (uint8_t i=0; i<3; ++i)
      {
        if ((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i)))
        {
          if (buttons & (0x1<<i))
            handler->OnMouseUp(i+1);
          else
            handler->OnMouseDown(i+1);
        }
      }
      buttons = buffer[0];
      
    }
  }
  
  return esp;
}

void MouseDriver::Activate()
{
  offset = 0; // between 0-2 try and error
  buttons = 0;

  commandport.Write(0xA8); // activate mouse interrupt
  commandport.Write(0x20); // get current state
  uint8_t status = dataport.Read() | 2;
  commandport.Write(0x60); // set state
  dataport.Write(status);

  commandport.Write(0xD4);
  dataport.Write(0xF4);
  dataport.Read();
}

