#ifndef VIDEO_HEADER_FILE
#define VIDEO_HEADER_FILE

// Internal method interfaces
int mediaplayer_generate_texture();
void mediaplayer_render_texture(void* ptrVideoTexId);
void mediaplayer_setup(char* file);
void mediaplayer_setup_video_track();
int mediaplayer_decode_packet ();
void mediaplayer_stop ();
void mediaplayer_pause ();
void mediaplayer_play ();
void mediaplayer_destroy ();
int mediaplayer_get_state();

int mediaplayer_get_audio_channels();
int mediaplayer_get_audio_sample_rate();

#endif