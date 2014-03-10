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

#include "avrlib/devices/mcp492x.h"
#include "avrlib/gpio.h"
#include "avrlib/boot.h"

using namespace avrlib;

SpiMaster<NumberedGpio<10>, MSB_FIRST, 2> pot_interface;

volatile uint16_t ramp;
int main(void) {
  Boot(false);
  pot_interface.Init();
  uint8_t a = 0;
  uint8_t b = 0;
  while (1) {
    pot_interface.WriteWord(0x00, a);
    pot_interface.WriteWord(0x10, b >= 128 ? (255 - (b << 1)) : (b << 1));
    ++a;
    ++b;
  }
}
