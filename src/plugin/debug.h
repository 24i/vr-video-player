#ifndef DEBUG_HEADER_FILE
#define DEBUG_HEADER_FILE

typedef void (*FuncPtr)( char * );
typedef void (*FuncPtrAudio)();
FuncPtr Debug;
FuncPtrAudio AudioData;
void Debugf(const char * format, ...);

#endif