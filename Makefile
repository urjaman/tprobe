# AVR-GCC Makefile
PROJECT=tprobe
SOURCES=main.c bootcheck.c probeuart.S uart.c adc.c
DEPS=Makefile main.h probeuart.h uart.h adc.h
CC=avr-gcc
OBJCOPY=avr-objcopy
LD=avr-ld
MMCU=attiny85
#AVRBINDIR=~/avr-tools/bin/
AVRDUDEMCU=t85
AVRDUDECMD=avrdude -p $(AVRDUDEMCU) -c atmelice_isp -P usb
DFLAGS=-DF_CPU=8000000
CFLAGS=-mmcu=$(MMCU) -Os -g -Wall -W -pipe -std=gnu99 -Wno-main $(DFLAGS) -Tavr25.x
SERIAL_DEV=/dev/ttyUSB0

all: $(PROJECT).hex $(PROJECT).bin size

$(PROJECT).hex: $(PROJECT).out
	$(AVRBINDIR)$(OBJCOPY) -j .text -j .data -j .bootloader -O ihex $(PROJECT).out $(PROJECT).hex

$(PROJECT).out: $(SOURCES) $(DEPS) boot.S
	$(AVRBINDIR)$(CC) $(CFLAGS) -flto -fwhole-program -flto-partition=none -mrelax -Wl,--section-start=.bootloader=0x1F00 -I./ -o $(PROJECT).out  $(SOURCES) boot.S -lc -lm

$(PROJECT).bin: $(PROJECT).out
	$(AVRBINDIR)$(OBJCOPY) -j .text -j .data -O binary $(PROJECT).out $(PROJECT).bin

asm: $(SOURCES)
	$(AVRBINDIR)$(CC) -S $(CFLAGS) -I./ -o $(PROJECT).S $(SOURCES)

objdump: $(PROJECT).out
	$(AVRBINDIR)avr-objdump -xd $(PROJECT).out | less

fullprogram: $(PROJECT).hex
	$(AVRBINDIR)$(AVRDUDECMD) -U efuse:w:0xFE:m -U hfuse:w:0xD6:m -U lfuse:w:0x52:m -U flash:w:$(PROJECT).hex


size: $(PROJECT).out
	$(AVRBINDIR)avr-size $(PROJECT).out

clean:
	-rm -f *.o
	-rm -f $(PROJECT).out
	-rm -f $(PROJECT).hex
	-rm -f $(PROJECT).S

picocom:
	picocom -b 115200 -f n -y n -d 8 -p 1 -e c  $(SERIAL_DEV)


program: $(PROJECT).bin serialprogrammer
	./serialprogrammer $(PROJECT).bin $(SERIAL_DEV)

serialprogrammer: serialprogrammer.c
	gcc -W -Wall -Os -o serialprogrammer serialprogrammer.c
