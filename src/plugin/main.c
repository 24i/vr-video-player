#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <libavformat/avformat.h>
#include <OpenGL/glu.h>

#include "unity/IUnityGraphics.h"
#include "debug.h"
#include "player.h"
#include "player/track.h"
#include "player/grabber/main.h"
#include "player/queue/video.h"
#include "player/queue/audio.h"

void* ptrVideoTexId = NULL;
int firstVideoRender = 0;

void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces) {
    av_register_all();
}

void SetUnityTexture (void* unityTexId) {
    ptrVideoTexId = unityTexId;
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventId) {
    if (mediaplayer_video_queue_get_size() > 50 && mediaplayer_get_state() == MEDIA_PLAYER_PLAYING) {
        MediaPlayerVideoQueueNode* node = mediaplayer_video_queue_pop();
        MediaPlayerTrack* videoTrack = mediaplayer_get_video_track();

        GLuint gltex = (GLuint)(size_t)(ptrVideoTexId);
        glBindTexture(GL_TEXTURE_2D, gltex);
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            videoTrack->codecContext->width,
            videoTrack->codecContext->height,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            node->data
        );

        av_freep(&node->data);
        free(node);
    }
}

void OnReadAudio (void * ptrAudioData) {
    // TODO: set audio data
}

UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc () {
    return OnRenderEvent;
}

