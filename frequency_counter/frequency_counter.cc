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

#include "avrlib/devices/buffered_display.h"
#include "avrlib/devices/hd44780_lcd.h"
#include "avrlib/gpio.h"
#include "avrlib/parallel_io.h"
#include "avrlib/boot.h"
#include "avrlib/time.h"
#include "avrlib/string.h"

using namespace avrlib;

typedef Hd44780Lcd<
    Gpio<PortD, 0>,
    Gpio<PortD, 1>,
    ParallelPort<PortD, PARALLEL_NIBBLE_HIGH> > Lcd;

Lcd lcd;
BufferedDisplay<Lcd> display;
static uint8_t cpt;

Timer<1> frequency_timer;
Timer<2> lcd_refresh_timer;
Gpio<PortC, 0> input_pin;

ISR(TIMER2_OVF_vect, ISR_NOBLOCK) {
  cpt = (cpt + 1) & 1;
  if (cpt == 0) {
    lcd.Tick();
  }
}

volatile uint32_t interval = 1;
volatile uint8_t num_overflows = 0;

ISR(TIMER1_CAPT_vect) {
  static uint16_t previous_time = 0;
  uint16_t current_time = ICR1;
  interval = current_time + \
      (static_cast<uint32_t>(num_overflows) << 16) - previous_time;
  previous_time = current_time;
  num_overflows = 0;
}

ISR(TIMER1_OVF_vect, ISR_NOBLOCK) {
  ++num_overflows;
}

int main(void) {
  Boot(false);
  lcd.Init();
  
  lcd_refresh_timer.set_prescaler(2);
  lcd_refresh_timer.set_mode(TIMER_PWM_PHASE_CORRECT);
  lcd_refresh_timer.Start();
  
  frequency_timer.set_prescaler(1);
  frequency_timer.set_mode(TIMER_NORMAL);
  frequency_timer.Start();
  frequency_timer.StartInputCapture();

  display.Init();
  display.Print(0, "frequency:");
  char buffer[16];
  float smoothed_frequency = 0.0;
  while (1) {
    display.Tick();
    if (interval) {
      float frequency = (F_CPU) / static_cast<float>(interval);
      smoothed_frequency = 0.95 * smoothed_frequency + 0.05 * frequency;
      memset(buffer, ' ', sizeof(buffer));
      sprintf(buffer, "%.3f", smoothed_frequency);
      display.Print(1, buffer);
    }
  }
}
