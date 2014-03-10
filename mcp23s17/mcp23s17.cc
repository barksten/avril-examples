// Copyright 2010 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "avrlib/devices/shift_register.h"
#include "avrlib/gpio.h"
#include "avrlib/boot.h"

using namespace avrlib;

NumberedGpio<13> blink;
typedef NumberedGpio<0> CS;
typedef NumberedGpio<1> SCK;
typedef NumberedGpio<2> SI;

ShiftRegisterOutput<CS, SCK, SI, 8, MSB_FIRST> reg;

void Mcp23s17Write(uint8_t address, uint8_t data) {
  reg.Begin();
  reg.ShiftOut(0x40);
  reg.ShiftOut(address);
  reg.ShiftOut(data);
  reg.End();
}

int main(void) {
  Boot(false);
  reg.Init();
  Mcp23s17Write(0 /* IODIRA */, 0);
  Mcp23s17Write(1 /* IODIRB */, 0);
  Mcp23s17Write(2 /* IOPOLA */, 0);
  Mcp23s17Write(3 /* IOPOLB */, 0);
  uint8_t counter = 0;
  while (1) {
    counter = counter + 1;
    Mcp23s17Write(0x12 /* GPIOA */, counter);
    Mcp23s17Write(0x13 /* GPIOB */, counter);
    _delay_ms(50);
  }
}
