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

// SpiMaster<NumberedGpio<10>, MSB_FIRST, 2> dac_interface;
UartSpiMaster<UartSpiPort0, NumberedGpio<10>, MSB_FIRST, 2> dac_interface;

NumberedGpio<2> flag;

volatile uint16_t ramp;

TIMER_2_TICK {
  flag.High();
  dac_interface.OptimisticWait();
  dac_interface.Strobe();
  ramp += 1024;
  uint16_t value = ramp >> 4;
  dac_interface.Overwrite(0x30 | (value >> 8));
  dac_interface.OptimisticWait();
  dac_interface.Overwrite(value & 0xff);
  flag.Low();
}

int main(void) {
  Boot(false);
  dac_interface.Init();
  flag.set_mode(DIGITAL_OUTPUT);
  Timer<2>::set_prescaler(1);
  Timer<2>::set_mode(TIMER_PWM_PHASE_CORRECT);
  dac_interface.WriteWord(0, 0);
  Timer<2>::Start();
  while (1) {
  }
}
