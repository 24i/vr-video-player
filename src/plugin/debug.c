#include <stdio.h>
#include <stdarg.h>

#include "debug.h"

void SetDebugCallback (FuncPtr ptrFunc) {
    Debug = ptrFunc;
}

void SetAudioCallback (FuncPtrAudio ptrFunc) {
    AudioData = ptrFunc;
}

void Debugf(const char * format, ...) {
    va_list args;
    char str[255];

    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);

    Debug(str);
}