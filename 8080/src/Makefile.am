lib_LTLIBRARIES = libemu_8080.la
# -Werror 
libemu_8080_la_SOURCES = emu-8080.c emu-8080.h  emu-8080.c.h
libemu_8080_la_CFLAGS = -Wall  -Wextra  -Walloc-zero  -Walloca -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment
include_HEADERS =  emu-8080.h
libemu_8080_la_LDFLAGS = -version-info 0:0:0

#libgettext_la_LDADD =
