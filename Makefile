FFMPEG_LIBS =    libavformat                        \
                 libavcodec                         \
                 libswscale                         \

CFLAGS += -Wall -O2 -g
CFLAGS := $(shell /usr/local/bin/pkg-config --cflags $(FFMPEG_LIBS)) $(CFLAGS)
CFLAGS := -lavformat -lavcodec -lswscale

lib:
	gcc -o main $(CFLAGS) main.c

osx:
	gcc $(CFLAGS) -arch x86_64 -bundle -Wl,-undefined,dynamic_lookup -lGLEW -o ./bin/Assets/Plugins/main.bundle ./src/plugin/main.c
