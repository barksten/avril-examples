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

#include <stdio.h>

#include "avrlib/gpio.h"
#include "avrlib/boot.h"
#include "avrlib/time.h"
#include "avrlib/timer.h"

using namespace avrlib;

typedef Timer<1> OscillatorTimer;
typedef Gpio<PortB, 2> OscillatorOut;
typedef PwmChannel1B Oscillator;

int main(void) {
  Boot(false);
  
  OscillatorOut::set_mode(DIGITAL_OUTPUT);
  OscillatorTimer::set_mode(_BV(WGM10), _BV(WGM13), 2);
  Oscillator::Start();
  Oscillator::set_frequency(16000000L / (2 * 8 * 440L));
  while (1) {
  }
}
