bin_PROGRAMS = emu_8080
emu_8080_SOURCES = main.c emu.c emu-8080.c emu-ports.c emu-screen.c
emu_8080_CFLAGS = $(GTK3_CFLAGS) -Werror -Wall  -Wextra  -Walloc-zero  -Walloca -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment
emu_8080_LDADD = $(GTK3_LIBS)
