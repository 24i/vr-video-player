#ifndef MEDIAPLAYER_TRACK_HEADER_FILE
#define MEDIAPLAYER_TRACK_HEADER_FILE

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

typedef struct MediaPlayerTrack {

    int trackIndex;

    AVStream *stream;
    AVCodec *codec;
    AVCodecContext *codecContext;

} MediaPlayerTrack;

void mediaplayer_setup_track (MediaPlayerTrack **track, AVFormatContext *ptrFormatContext, enum AVMediaType type);

#endif