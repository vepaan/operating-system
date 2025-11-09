#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "types.h"
#include "interrupts.h"
#include "driver.h"
#include "port.h"

class KeyboardEventHandler
{
public:
  KeyboardEventHandler();
  virtual void OnKeyDown(char);
  virtual void OnKeyUp(char);
};

class PrintfKeyboardEventHanlder: public KeyboardEventHandler
{
public:
  void OnKeyDown(char c);
};

class KeyboardDriver: public InterruptHandler, public Driver
{
  Port8Bit dataport;
  Port8Bit commandport;
  KeyboardEventHandler* handler;
public:
  KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* handler);
  ~KeyboardDriver();
  virtual uint32_t HandleInterrupt(uint32_t esp);
  virtual void Activate();
};

#endif // __KEYBOARD_H
