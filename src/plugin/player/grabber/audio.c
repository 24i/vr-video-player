#include <libavutil/opt.h>
#include <libswresample/swresample.h>

#include "../track.h"
#include "../queue/audio.h"
#include "../../debug.h"

struct SwrContext* swrContext;

void mediaplayer_audio_grabber_set_swr_context (MediaPlayerTrack *audioTrack) {
    swrContext = swr_alloc();
    av_opt_set_int(swrContext, "in_channel_layout",  audioTrack->codecContext->channel_layout, 0);
    av_opt_set_int(swrContext, "out_channel_layout", audioTrack->codecContext->channel_layout,  0);
    av_opt_set_int(swrContext, "in_sample_rate",     audioTrack->codecContext->sample_rate, 0);
    av_opt_set_int(swrContext, "out_sample_rate",    audioTrack->codecContext->sample_rate, 0);
    av_opt_set_sample_fmt(swrContext, "in_sample_fmt",  audioTrack->codecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(swrContext, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
    swr_init(swrContext);
}

void mediaplayer_grab_audio_frame (AVFrame *ptrFrame) {
    uint8_t **ptrAudioData = NULL;

    // Allocate audio sample data
    av_samples_alloc_array_and_samples(
        &ptrAudioData,
        NULL,
        ptrFrame->channels,
        ptrFrame->nb_samples,
        AV_SAMPLE_FMT_FLT,
        0
    );

    // Convert to understandable format
    int ret = swr_convert(
        swrContext,
        ptrAudioData,
        ptrFrame->nb_samples,
        (uint8_t const **)ptrFrame->extended_data,
        ptrFrame->nb_samples
    );

    if (ret < 0) {
        Debug("Error while converting audio");
        return;
    }

    int dst_bufsize = av_samples_get_buffer_size(NULL, ptrFrame->channels, ret, AV_SAMPLE_FMT_FLT, 1);
    //AudioData(ptrAudioData[0], dst_bufsize);
    //

    mediaplayer_audio_queue_enqueue(ptrAudioData[0], dst_bufsize);
}