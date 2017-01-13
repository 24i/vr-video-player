#ifndef VIDEO_HEADER_FILE
#define VIDEO_HEADER_FILE

// Internal method interfaces
int vr_player_generate_texture();
void vr_player_render_texture(void* ptrVideoTexId);
void vr_player_setup(char* file);
void vr_player_setup_video_track();
int vr_decode_packet ();
void vr_player_stop ();
void vr_player_pause ();
void vr_player_play ();
void vr_player_destroy ();
int vr_player_get_state();

// Internal structure interfaces
typedef struct VRPlayerVideoData {

    int videoWidth;
    int videoHeight;

} VRPlayerVideoData;

#endif