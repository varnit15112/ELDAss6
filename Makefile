comp = avr-gcc
mmcu1 = atmega328p
mmcu2 = -mmcu=$(mmcu1) -Os
file_name = deb
port = /dev/cu.usbserial-A7005z17
baud_rate = 57600


all:  program

compile: $(file_name).hex

$(file_name).hex: $(file_name).elf
	avr-objcopy -O ihex $(file_name).elf $(file_name).hex


$(file_name).elf: $(file_name).c
	$(comp) $(mmcu2) -o $(file_name).elf $(file_name).c

clean: 
	rm -rf *.o *.elf *.hex

program: compile
	avrdude -c arduino -p $(mmcu1) -P $(port) -b $(baud_rate) -U flash:w:$(file_name).hex