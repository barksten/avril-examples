# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

EXAMPLES=blink eeprom encoder frequency_counter frequency_generator lcd mcp4921 parallel scan_pots serial_debug switches_and_leds

ELF0=$(addsuffix .elf,$(EXAMPLES))
ELF1=$(addprefix /,$(ELF0))
ELF2=$(join $(EXAMPLES),$(ELF1))
ELF3=$(addprefix build/,$(ELF2))

all: $(ELF3)

clean:
	$(RM) -r $(addprefix build/,$(EXAMPLES))

$(ELF3):
	$(MAKE) -f $(basename $(notdir $@))/makefile
