
.PHONY: clean re all

all: invaders.rom.h

invaders.rom.h: invaders.rom
	xxd -i invaders.rom > invaders.rom.h

invaders.rom: invaders.e invaders.f invaders.g invaders.h
	cat invaders.h invaders.g invaders.f invaders.e > invaders.rom

clean:
	rm -f invaders.rom invaders.rom.h

re: clean invaders.rom.h
