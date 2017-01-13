FFMPEG_LIBS =    libavformat                        \
                 libavcodec                         \
                 libswscale                         \
                 libswresample						\

CFLAGS += -Wall -O2 -g
CFLAGS := $(shell /usr/local/bin/pkg-config --cflags $(FFMPEG_LIBS)) $(CFLAGS)
CFLAGS := -lavformat -lavcodec -lswscale

CFILES = $(shell find ./src -type f -name '*.c')

%:
	$(shell cp ./src/unity/MediaPlayer.cs ./bin/Assets/MediaPlayer.cs)

osx:
	$(shell cp ./src/unity/MediaPlayer.cs ./bin/Assets/MediaPlayer.cs)
	gcc $(CFLAGS) -arch x86_64 -bundle -Wl,-undefined,dynamic_lookup -lGLEW -o ./bin/Assets/Plugins/main.bundle $(CFILES)

android:
	ndk-build --directory=./ NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=Android.mk