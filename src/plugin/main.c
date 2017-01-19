#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <libavformat/avformat.h>

#include "unity/IUnityGraphics.h"
#include "debug.h"
#include "player.h"

void* ptrVideoTexId = NULL;
int firstVideoRender = 0;

void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces) {
    av_register_all();
}

void SetUnityTexture (void* unityTexId) {
    ptrVideoTexId = unityTexId;
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventId) {
    if (mediaplayer_generate_texture() >= 0) {
        mediaplayer_render_texture(ptrVideoTexId);
    }
}

UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc () {
    return OnRenderEvent;
}

