#ifndef AUDIO_QUEUE_HEADER
#define AUDIO_QUEUE_HEADER

typedef struct MediaPlayerAudioQueueNode {

    uint8_t *data;
    int buffer_size;
    struct MediaPlayerAudioQueueNode* next;
    int index;

} MediaPlayerAudioQueueNode;

void mediaplayer_audio_queue_enqueue (uint8_t *data, int buffer_size);
MediaPlayerAudioQueueNode* mediaplayer_audio_queue_pop ();
int mediaplayer_audio_queue_get_size ();

#endif