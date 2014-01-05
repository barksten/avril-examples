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
