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
#include <avr/pgmspace.h>

#include "avrlib/adc.h"
#include "avrlib/boot.h"
#include "avrlib/gpio.h"
#include "avrlib/time.h"
#include "avrlib/timer.h"

using namespace avrlib;

Timer<1> oscillator_timer;
Gpio<PortB, 2> oscillator_out;
PwmChannel1B oscillator;
Adc adc;

static const uint16_t kOctave = 12 << 7;

prog_uint16_t pitch_table[] PROGMEM = {
  61156, 61046, 60936, 60826, 60716, 60607, 60497, 60388,
  60279, 60171, 60062, 59954, 59846, 59738, 59630, 59522,
  59415, 59308, 59201, 59094, 58988, 58881, 58775, 58669,
  58563, 58458, 58352, 58247, 58142, 58037, 57932, 57828,
  57724, 57620, 57516, 57412, 57308, 57205, 57102, 56999,
  56896, 56793, 56691, 56589, 56487, 56385, 56283, 56182,
  56080, 55979, 55878, 55778, 55677, 55577, 55476, 55376,
  55276, 55177, 55077, 54978, 54879, 54780, 54681, 54582,
  54484, 54386, 54288, 54190, 54092, 53994, 53897, 53800,
  53703, 53606, 53509, 53413, 53316, 53220, 53124, 53028,
  52933, 52837, 52742, 52647, 52552, 52457, 52363, 52268,
  52174, 52080, 51986, 51892, 51799, 51705, 51612, 51519,
  51426, 51333, 51241, 51148, 51056, 50964, 50872, 50780,
  50689, 50597, 50506, 50415, 50324, 50233, 50143, 50052,
  49962, 49872, 49782, 49692, 49603, 49513, 49424, 49335,
  49246, 49157, 49068, 48980, 48891, 48803, 48715, 48627,
  48540, 48452, 48365, 48277, 48190, 48104, 48017, 47930,
  47844, 47757, 47671, 47585, 47500, 47414, 47328, 47243,
  47158, 47073, 46988, 46903, 46819, 46734, 46650, 46566,
  46482, 46398, 46314, 46231, 46147, 46064, 45981, 45898,
  45815, 45733, 45650, 45568, 45486, 45404, 45322, 45240,
  45158, 45077, 44996, 44915, 44834, 44753, 44672, 44591,
  44511, 44431, 44351, 44271, 44191, 44111, 44032, 43952,
  43873, 43794, 43715, 43636, 43557, 43479, 43400, 43322,
  43244, 43166, 43088, 43010, 42933, 42855, 42778, 42701,
  42624, 42547, 42470, 42394, 42317, 42241, 42165, 42089,
  42013, 41937, 41861, 41786, 41711, 41635, 41560, 41485,
  41410, 41336, 41261, 41187, 41113, 41038, 40964, 40891,
  40817, 40743, 40670, 40596, 40523, 40450, 40377, 40304,
  40232, 40159, 40087, 40014, 39942, 39870, 39798, 39726,
  39655, 39583, 39512, 39441, 39370, 39299, 39228, 39157,
  39086, 39016, 38945, 38875, 38805, 38735, 38665, 38596,
  38526, 38456, 38387, 38318, 38249, 38180, 38111, 38042,
  37974, 37905, 37837, 37769, 37700, 37632, 37565, 37497,
  37429, 37362, 37294, 37227, 37160, 37093, 37026, 36959,
  36893, 36826, 36760, 36693, 36627, 36561, 36495, 36429,
  36364, 36298, 36233, 36167, 36102, 36037, 35972, 35907,
  35842, 35778, 35713, 35649, 35584, 35520, 35456, 35392,
  35328, 35265, 35201, 35138, 35074, 35011, 34948, 34885,
  34822, 34759, 34696, 34634, 34571, 34509, 34447, 34385,
  34323, 34261, 34199, 34137, 34076, 34014, 33953, 33892,
  33831, 33770, 33709, 33648, 33587, 33527, 33466, 33406,
  33346, 33285, 33225, 33166, 33106, 33046, 32986, 32927,
  32868, 32808, 32749, 32690, 32631, 32572, 32513, 32455,
  32396, 32338, 32280, 32221, 32163, 32105, 32047, 31990,
  31932, 31874, 31817, 31759, 31702, 31645, 31588, 31531,
  31474, 31417, 31361, 31304, 31248, 31191, 31135, 31079,
  31023, 30967, 30911, 30855, 30800, 30744, 30689, 30633,
  30578 };

uint16_t note_to_count(int16_t note) {
  // Lowest note: C0.
  note -= kOctave;
  // Transpose the lowest octave up.
  while (note < 0) {
    note += kOctave;
  }
  uint8_t shifts = 0;
  while (note >= kOctave) {
    note -= kOctave;
    ++shifts;
  }
  
  uint16_t index_integral = note >> 2;
  uint16_t index_fractional = note & 0x3;
  uint16_t a = pgm_read_word(pitch_table + index_integral);
  while (shifts--) {
    a >>= 1;
  }
  return a;
}

int main(void) {
  Boot(false);

  adc.Init();

  oscillator_out.set_mode(DIGITAL_OUTPUT);
  oscillator_timer.set_mode(_BV(WGM10), _BV(WGM13), 2);
  oscillator.Start();
  uint16_t midi_note = 57 << 7;  // A3
  while (1) {
    uint16_t value = adc.Read(0) << 3;
    oscillator.set_frequency(note_to_count(value + (45 << 7)));
  }
}
