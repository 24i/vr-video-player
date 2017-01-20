#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>

#include "video.h"
#include "../track.h"
#include "../queue/video.h"
#include "../../debug.h"

// Video converter context
struct SwsContext* swsContext;

/**
 * Set context used for converting the actual video frame
 * @param ptrContext The initialised SwsContext
 */
void mediaplayer_video_grabber_set_sws_context (MediaPlayerTrack *ptrVideoTrack) {
    swsContext = sws_getContext(
        ptrVideoTrack->codecContext->width,
        ptrVideoTrack->codecContext->height,
        AV_PIX_FMT_YUV420P,
        ptrVideoTrack->codecContext->width,
        ptrVideoTrack->codecContext->height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );
}

/**
 * Grabs and decodes a video frame
 * @return Error code or size of decoded data
 */
void mediaplayer_grab_video_frame (AVCodecContext *ptrCodecContext, AVFrame *ptrFrame) {

    // Video data container
    uint8_t *ptrVideoData[4];

    int linesize[4];

    // Allocate image data
    av_image_alloc(
        ptrVideoData,
        linesize,
        ptrCodecContext->width,
        ptrCodecContext->height,
        AV_PIX_FMT_RGB24,
        1
    );


    // Scale & convert the frame data
    sws_scale(
        swsContext,
        (uint8_t const * const *)(ptrFrame->data),
        ptrFrame->linesize, 0,
        ptrCodecContext->height,
        ptrVideoData,
        linesize
    );

    mediaplayer_video_queue_enqueue(ptrVideoData[0]);
}