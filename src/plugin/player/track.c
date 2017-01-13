#include "track.h"
#include "../debug.h"

void vr_player_setup_track (VRPlayerTrack **track, AVFormatContext *ptrFormatContext, enum AVMediaType type) {
    VRPlayerTrack *tmpTrack = (VRPlayerTrack *)malloc(sizeof(VRPlayerTrack));
    int stream = av_find_best_stream(ptrFormatContext, type, -1, -1, NULL, 0);

    if (stream < 0) {
        Debug("No suitable track found for media type");
        return;
    }

    // Setup base track properties
    tmpTrack->trackIndex = stream;
    tmpTrack->stream = ptrFormatContext->streams[tmpTrack->trackIndex];
    tmpTrack->codec = avcodec_find_decoder(tmpTrack->stream->codecpar->codec_id);

    if (tmpTrack->codec == NULL) {
        Debug("Decoder not found for media type");
        return;
    }

    // Setup codec context
    tmpTrack->codecContext = avcodec_alloc_context3(tmpTrack->codec);
    if (tmpTrack->codecContext == NULL) {
        Debug("Decoder context could not be initialized for media type");
        return;
    }
    avcodec_parameters_to_context(tmpTrack->codecContext, tmpTrack->stream->codecpar);

    avcodec_open2(tmpTrack->codecContext, tmpTrack->codec, NULL);

    // Make sure the track pointer is referencing the new track in memory
    *track = tmpTrack;

    return;
}