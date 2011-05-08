DEVICE = atmega328p
AVRDUDE_DEVICE = m328p
DEVICE ?= atmega168
AVRDUDE_DEVICE ?= m168

CFLAGS=-g -Wall -mcall-prologues -mmcu=$(DEVICE) -Os -I../../lib/
CC=avr-gcc
OBJ2HEX=avr-objcopy 
LDFLAGS=-Wl,-gc-sections -lpololu_$(DEVICE) -Wl,-relax

PORT ?= /dev/orangutan_programmer
AVRDUDE=avrdude
TARGET=linefollower-contest
OBJECT_FILES=linefollower-contest.o follow-segment.o

all: $(TARGET).hex

clean:
	rm -f *.o *.hex *.obj *.hex

%.hex: %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

%.obj: $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(OBJECT_FILES) $(LDFLAGS) -o $@

program: $(TARGET).hex
	$(AVRDUDE) -p $(AVRDUDE_DEVICE) -c avrisp2 -P $(PORT) -U flash:w:$(TARGET).hex
