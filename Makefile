# Makefile for paranoia 2.0

CFLAGS = -std=c99 -s -pedantic -Wall -Wextra -Wfatal-errors -pedantic-errors -O3 -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200809L
CC     = gcc $(CFLAGS)

binaries  = art cmd dat eye idx inf qtz tmb tvs uzx www zwt
libraries = libmd5.so libthumb.so libsoldout.so libparanoia.so


all: $(libraries) $(binaries)


libsoldout.so:
	$(CC) -shared -fPIC src/lib/soldout/markdown.c \
		src/lib/soldout/array.c \
		src/lib/soldout/buffer.c \
		src/lib/soldout/renderers.c \
		-o libsoldout.so

libmd5.so:
	$(CC) -shared -fPIC src/lib/md5/md5.c -o libmd5.so

libthumb.so:
	$(CC) -shared -fPIC src/lib/thumb/thumb.c \
		-lpng src/lib/thumb/thumb_png.c \
		-ljpeg src/lib/thumb/thumb_jpeg.c \
		-o libthumb.so

libparanoia.so:
	$(CC) -shared -fPIC src/lib/paranoia/paranoia.c -o libparanoia.so


art: libparanoia.so
	$(CC) -L. -Wl,-rpath=. -lfcgi -lcurl -lparanoia -ljson-c src/art.c -o art

cmd: libparanoia.so
	$(CC) -L. -Wl,-rpath=. -lfcgi -lparanoia -ljson-c src/cmd.c -o cmd

dat: libparanoia.so
	$(CC) -L. -Wl,-rpath=. -lfcgi -lparanoia src/dat.c -o dat

eye: libparanoia.so
	$(CC) -L. -Wl,-rpath=. -lfcgi -lcurl -lparanoia src/eye.c -o eye

idx: libparanoia.so libsoldout.so
	$(CC) -L. -Wl,-rpath=. -lfcgi -lparanoia -lsoldout -ljson-c src/idx.c -o idx

inf: libparanoia.so
	$(CC) -L. -Wl,-rpath=. -lfcgi -lparanoia -ljson-c src/inf.c -o inf

qtz: libparanoia.so
	$(CC) -L. -Wl,-rpath=. -lfcgi -lparanoia -ljson-c src/qtz.c -o qtz

tmb: libmd5.so libthumb.so
	$(CC) -L. -Wl,-rpath=. -lfcgi -lcurl -lm -lmd5 -lthumb src/tmb.c -o tmb

tvs: libparanoia.so
	$(CC) -L. -Wl,-rpath=. -lfcgi -lparanoia -ljson-c src/tvs.c -o tvs

uzx: libparanoia.so
	$(CC) -L. -Wl,-rpath=. -lfcgi -lparanoia -ljson-c src/uzx.c -o uzx

www: libparanoia.so
	$(CC) -L. -Wl,-rpath=. -lfcgi -lparanoia -ljson-c src/www.c -o www

zwt: libparanoia.so
	$(CC) -L. -Wl,-rpath=. -lfcgi -lparanoia -ljson-c src/zwt.c -o zwt


# Remove compiled binaries and libraries
clean: tidy
	rm -f $(libraries) $(binaries)

# Remove any plaintext back-ups
tidy:
	rm -f src/*~

# Get rid of the thumbnail cache
flush:
	rm -f t/*


.PHONY: all clean tidy flush
