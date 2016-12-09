typedef void (*FuncPtr)( char * );
FuncPtr Debug;

void SetDebugCallback (FuncPtr ptrFunc) {
    Debug = ptrFunc;
}

void Debugf(const char * format, ...) {
    va_list args;
    char str[255];

    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);

    Debug(str);
}