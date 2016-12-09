#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>

#include <OpenGL/glu.h>

#include "unity/IUnityGraphics.h"
//#include "unity/IUnityInterface.h"

void* ptrVideoTexId = NULL;
int firstVideoRender = 0;

#include "debug.c"
#include "video.c"

void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces) {
    av_register_all();
}

void SetUnityTexture (void* unityTexId) {
    ptrVideoTexId = unityTexId;
}

void Play () {
    vrVideoPlayerState = VR_VIDEO_PLAYER_PLAYING;
}

void RenderTexture () {

    // Don't render anything without a texture to render on
    if (ptrVideoTexId == NULL) {
        return;
    }

    if (ptrVideoData[0] == NULL) {
        return;
    }

    GLuint gltex = (GLuint)(size_t)(ptrVideoTexId);
    glBindTexture(GL_TEXTURE_2D, gltex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoWidth, videoHeight, GL_RGB, GL_UNSIGNED_BYTE, ptrVideoData[0]);

}

static void UNITY_INTERFACE_API OnRenderEvent(int eventId) {
    if (GenerateVideoTexture() >= 0) {
        RenderTexture();
    }
}

UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc() {
    return OnRenderEvent;
}