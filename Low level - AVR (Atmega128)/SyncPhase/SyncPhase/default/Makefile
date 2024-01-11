###############################################################################
# Makefile for the project SyncPhase
###############################################################################

## General Flags
PROJECT = SyncPhase
MCU = atmega128
TARGET = SyncPhase.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2       -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += 


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = SyncPhase.o Phase.o Timer.o Signal.o Setting.o crc8.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) SyncPhase.hex SyncPhase.eep size

## Compile
SyncPhase.o: ../SyncPhase.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Phase.o: ../Phase.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Timer.o: ../Timer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Signal.o: ../Signal.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Setting.o: ../Setting.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

crc8.o: ../crc8.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) SyncPhase.elf dep/* SyncPhase.hex SyncPhase.eep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

