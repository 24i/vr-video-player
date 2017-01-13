#ifndef VRTRACK_HEADER_FILE
#define VRTRACK_HEADER_FILE

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

typedef struct VRPlayerTrack {

    int trackIndex;

    AVStream *stream;
    AVCodec *codec;
    AVCodecContext *codecContext;

} VRPlayerTrack;

void vr_player_setup_track (VRPlayerTrack **track, AVFormatContext *ptrFormatContext, enum AVMediaType type);

#endif