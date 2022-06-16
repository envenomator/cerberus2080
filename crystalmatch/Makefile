STARTADDRESS	:= 0x0205
DATASTART		:= 0x2500
SERIALPORT		:= /dev/ttyUSB0
SEND_SCRIPT		:= ../tools/send.py

.PHONY:	all
all:

	# Compile crt0.s
	sdasz80 -xlos -g crt0.s

	# Compile console.c
	sdcc -o console.rel \
		-c --std-c11 -mz80 --debug -I /usr/share/sdcc/include \
		--nostdinc --no-std-crt0 --nostdinc --nostdlib \
		console.c
	# Compile random.c
	sdcc -o random.rel \
		-c -mz80 --debug -I /usr/share/sdcc/include \
		--nostdinc --no-std-crt0 --nostdinc --nostdlib \
		random.c
	# Compile queue.c
	sdcc -o queue.rel \
		-c -mz80 --debug -I /usr/share/sdcc/include \
		--nostdinc --no-std-crt0 --nostdinc --nostdlib \
		queue.c
	# Compile playfield.c
	sdcc -o playfield.rel \
		-c -mz80 --debug -I /usr/share/sdcc/include \
		--nostdinc --no-std-crt0 --nostdinc --nostdlib \
		playfield.c
	# Compile main.c
	sdcc -o main.rel \
		-c -mz80 --debug -I /usr/share/sdcc/include \
		--nostdinc --no-std-crt0 --nostdinc --nostdlib \
		main.c
		
	# Link all files
	sdcc -o main.ihx \
		-mz80 -Wl -y --code-loc 0x0205 --data-loc $(DATASTART)\
		--std-c11 -mz80 --debug\
		--no-std-crt0 --nostdinc --nostdlib \
		-Llibsdcc-z80 -lz80.lib \
		crt0.rel main.rel console.rel random.rel queue.rel playfield.rel
	
	# Finally, convert ihx to binary
	sdobjcopy -I ihex -O binary main.ihx main.bin

upload: all
	$(SEND_SCRIPT) main.bin $(SERIALPORT)

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
