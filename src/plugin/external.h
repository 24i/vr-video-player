#ifndef EXTERNAL_HEADER_FILE
#define EXTERNAL_HEADER_FILE

// Public video method interfaces
void MediaPlayerPlay();
void MediaPlayerPause();
void MediaPlayerSetup();
void MediaPlayerDestroy();
int MediaPlayerGetState();

float MediaPlayerGetDuration();
float MediaPlayerGetPosition();

int MediaPlayerGetAudioChannels();
int MediaPlayerGetAudioSampleRate();
#endif