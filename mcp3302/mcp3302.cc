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

#include "avrlib/boot.h"
#include "avrlib/gpio.h"
#include "avrlib/spi.h"

using namespace avrlib;

PwmOutput<3> led;
SpiMaster<SpiSS, MSB_FIRST, 64> spi;

uint16_t ReadADC(uint8_t channel) {
  uint8_t tx_1 = 0x08 | 0x04 | (channel >> 1); // Start bit, single ended.
  uint8_t tx_2 = (channel & 1) ? 0x80 : 0;
  Word rx;
  spi.Begin();
  spi.Send(tx_1);
  spi.Send(tx_2);  // Dummy
  rx.bytes[1] = spi.ImmediateRead() & 0xf;  // Discard null and sign bit.
  spi.Send(0x00);  // Dummy
  rx.bytes[0] = spi.ImmediateRead();
  spi.End();
  return rx.value;
}

int main(void) {
  Boot(false);
  Timer<2>::set_prescaler(1);
  Timer<2>::set_mode(TIMER_PWM_PHASE_CORRECT);
  led.Init();
  spi.Init();
  while (1) {
    uint16_t value = ReadADC(0);
    led.Write(value >> 4);
  }
}
