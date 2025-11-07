#ifndef __MOUSE_H
#define __MOUSE_H

#include "types.h"
#include "interrupts.h"
#include "port.h"

class MouseDriver: public InterruptHandler
{
  Port8Bit dataport;
  Port8Bit commandport;

  uint8_t buffer[3]; // how mouse data is received (X-axis movememnt->buffer[1], Y-axis movement->buffer[2])
  uint8_t offset; // can be 0, 1, or 2, just trial and error for your mouse
  uint8_t buttons;
public:
  MouseDriver(InterruptManager* manager);
  ~MouseDriver();
  virtual uint32_t HandleInterrupt(uint32_t esp);
};

#endif // __MOUSE_H
