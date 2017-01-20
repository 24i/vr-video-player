#include "external.h"
#include "player.h"

void MediaPlayerSetup(char* file) {
    mediaplayer_setup(file);
}

void MediaPlayerDestroy() {
    mediaplayer_destroy();
}

void MediaPlayerStop() {
    mediaplayer_stop();
}

void MediaPlayerPause() {
    mediaplayer_pause();
}

void MediaPlayerPlay() {
    mediaplayer_play();
}

int MediaPlayerGetState() {
    return mediaplayer_get_state();
}

float MediaPlayerGetDuration () {
    return mediaplayer_get_duration();
}

float MediaPlayerGetPosition () {
    return mediaplayer_get_position();
}

int MediaPlayerGetAudioChannels() {
    return mediaplayer_get_audio_channels();
}
int MediaPlayerGetAudioSampleRate() {
    return mediaplayer_get_audio_sample_rate();
}