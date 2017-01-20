#ifndef VIDEO_QUEUE_HEADER
#define VIDEO_QUEUE_HEADER

typedef struct MediaPlayerVideoQueueNode {

    uint8_t *data;
    struct MediaPlayerVideoQueueNode* next;
    int index;
    float timestamp;

} MediaPlayerVideoQueueNode;

void mediaplayer_video_queue_enqueue (uint8_t *data);
MediaPlayerVideoQueueNode* mediaplayer_video_queue_pop ();
MediaPlayerVideoQueueNode* mediaplayer_video_queue_first ();
int mediaplayer_video_queue_get_size ();

#endif