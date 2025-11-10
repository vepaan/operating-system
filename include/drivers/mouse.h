#ifndef __MYOS__DRIVERS__MOUSE_H
#define __MYOS__DRIVERS__MOUSE_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>


namespace myos {
  namespace drivers
  {
    class MouseEventHandler
    {
    public:
      MouseEventHandler();
      virtual void OnActivate();
      virtual void OnMouseDown(myos::common::uint8_t button);
      virtual void OnMouseUp(myos::common::uint8_t button);
      virtual void OnMouseMove(int x, int y);
    };

    class MouseToConsole: public MouseEventHandler
    {
    private:
      myos::common::int8_t x, y;

    public:
      MouseToConsole();
      void OnMouseMove(int x, int y);
    };

    class MouseDriver: public myos::hardwarecommunication::InterruptHandler, public Driver
    {
    private:

      myos::hardwarecommunication::Port8Bit dataport;
      myos::hardwarecommunication::Port8Bit commandport;

      myos::common::uint8_t buffer[3]; // how mouse data is received (X-axis movememnt->buffer[1], Y-axis movement->buffer[2])
      myos::common::uint8_t offset; // can be 0, 1, or 2, just trial and error for your mouse
      myos::common::uint8_t buttons;

      MouseEventHandler* handler;

    public:

      MouseDriver(myos::hardwarecommunication::InterruptManager* manager, MouseEventHandler* handler);
      ~MouseDriver();
      virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
      virtual void Activate();
    };
  }
}

#endif // __MYOS__DRIVERS__MOUSE_H
