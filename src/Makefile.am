bin_PROGRAMS = emu_invaders
# -Werror 
emu_invaders_SOURCES = main.c emu.c emu-ports.c emu-screen.c emu-bios.c emu-keyboard.c emu-shifter.c
emu_invaders_CFLAGS = $(GTK3_CFLAGS) -I../8080/src -fpic -Wall -Werror -Wextra -Walloca -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow
emu_invaders_LDADD = $(GTK3_LIBS) ../8080/src/libemu_8080.la
