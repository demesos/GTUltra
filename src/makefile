#
# Linux makefile
#
#   make            everything linked against the system SDL2 and ALSA (needs the
#                   runtime packages, e.g. libsdl2-2.0-0 and libasound2)
#   make STATIC=1   gt2reloc, ins2snd2, ss2stereo and mod2sng2 fully static, without
#                   any dependencies. gt2reloc gets SDL stubs from nosdl.c and the dummy
#                   MIDI backend; it needs neither for relocating. gtultra stays dynamic.
#   make STATIC=1 STATIC_SDL=1
#                   additionally links SDL2, libstdc++ and libgcc into gtultra. With the
#                   distribution's libSDL2.a this only removes libSDL2 itself from the
#                   dependencies (the X11/Wayland/audio libraries SDL2 uses stay). With an
#                   SDL2 built with runtime-loaded backends only glibc and libasound remain:
#                     make STATIC=1 STATIC_SDL=1 SDL2_CONFIG=/path/to/sdl2/bin/sdl2-config
#   Run "make clean" when switching between these variants.

SDL2_CONFIG?=sdl2-config
CFLAGS+=`$(SDL2_CONFIG) --cflags` `pkg-config --cflags alsa`
PREFIX=../linux/
SUFFIX=

ifeq ($(STATIC_SDL),1)
LIBS+=`$(SDL2_CONFIG) --static-libs` `pkg-config --libs alsa` -lpthread -static-libstdc++ -static-libgcc
else
LIBS+=`$(SDL2_CONFIG) --libs` `pkg-config --libs alsa` -lpthread
endif

ifeq ($(STATIC),1)
RELOC_OBJS=nosdl.o
RELOC_MIDI=RtMidi_nomidi.o
RELOC_LIBS=-static -lpthread
TOOL_LDFLAGS=-static
endif

include makefile.common
