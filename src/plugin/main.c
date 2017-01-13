#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <libavformat/avformat.h>

#include "unity/IUnityGraphics.h"
#include "debug.h"
#include "video.h"

void* ptrVideoTexId = NULL;
int firstVideoRender = 0;

void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces) {
    av_register_all();
}

void SetUnityTexture (void* unityTexId) {
    ptrVideoTexId = unityTexId;
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventId) {
    if (vr_player_generate_texture() >= 0) {
        vr_player_render_texture(ptrVideoTexId);
    }
}

UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc () {
    return OnRenderEvent;
}

