STARTADDRESS	:= 0x0205
DATASTART		:= 0x8000
SERIALPORT		:= /dev/ttyUSB0
SEND_SCRIPT		:= ../tools/send.py

.PHONY:	all
all:

	# Compile crt0.s
	sdasz80 -xlos -g crt0.s

	# Compile console.c
	sdcc -o console.rel \
		-c --std-c11 -mz80 --debug \
		--nostdinc --no-std-crt0 --nostdinc --nostdlib \
		console.c
	# Compile test.c
	sdcc -o test.rel \
		-c -mz80 --debug \
		--nostdinc --no-std-crt0 --nostdinc --nostdlib \
		test.c
		
	# Link both
	sdcc -o test.ihx \
		-mz80 -Wl -y --code-loc 0x0205 --data-loc 0x0300 \
		--std-c11 -mz80 --debug\
		--no-std-crt0 --nostdinc --nostdlib \
		-Llibsdcc-z80 -llibsdcc-z80 \
		crt0.rel test.rel console.rel
	
	# Finally, convert ihx to binary
	sdobjcopy -I ihex -O binary test.ihx test.bin

upload: all
	$(SEND_SCRIPT) test.bin $(SERIALPORT)

.PHONY: clean
clean:
	# Remoe everything except source files
	rm -f *.lst
	rm -f *.sym
	rm -f *.asm
	rm -f *.ihx
	rm -f *.lk
	rm -f *.noi	
	rm -f *.map
	rm -f *.rel
	rm -f *.?db	
	rm -f *.bin
