bin_PROGRAMS = emu_invaders
# -Werror 
emu_invaders_SOURCES = main.c emu.c emu-ports.c emu-screen.c
emu_invaders_CFLAGS = $(GTK3_CFLAGS) -I../8080/src -Wall  -Wextra  -Walloc-zero  -Walloca -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment
emu_invaders_LDADD = $(GTK3_LIBS) ../8080/src/.libs/libemu_8080.la
