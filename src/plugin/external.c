#include "external.h"
#include "player.h"

void VRVideoPlayerSetup(char* file) {
    vr_player_setup(file);
}

void VRVideoPlayerDestroy() {
    vr_player_destroy();
}

void VRVideoPlayerStop() {
    vr_player_stop();
}

void VRVideoPlayerPause() {
    vr_player_pause();
}

void VRVideoPlayerPlay() {
    vr_player_play();
}

int VRVideoPlayerGetState() {
    return vr_player_get_state();
}

int VRVideoPlayerGetAudioChannels() {
    return vr_player_get_audio_channels();
}
int VRVideoPlayerGetAudioSampleRate() {
    return vr_player_get_audio_sample_rate();
}