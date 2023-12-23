AM_LDFLAGS = -static-libasan

lib_LTLIBRARIES = libemu_8080.la
# -Werror 
libemu_8080_la_SOURCES = emu-8080.c emu-8080.h
#-fsanitize=address
libemu_8080_la_CFLAGS = -shared -fpic -Wall -Werror -Wextra -Walloca -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow
