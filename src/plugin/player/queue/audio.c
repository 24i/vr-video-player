#include <stdlib.h>

#include "audio.h"
#include "../../debug.h"

MediaPlayerAudioQueueNode* firstAudioNode = NULL;
MediaPlayerAudioQueueNode* lastAudioNode = NULL;

int audioQueueSize = 0;

void mediaplayer_audio_queue_enqueue (uint8_t *data, int buffer_size) {
    MediaPlayerAudioQueueNode* temp = malloc(sizeof(MediaPlayerAudioQueueNode));

    temp->data = data;
    temp->next = NULL;
    temp->buffer_size = buffer_size;
    temp->index = audioQueueSize; // Size is 0 based at this point

    /*
     * If nothing is in the queue yet the new node is both
     * the rear and front of the queue
     */
    if (firstAudioNode == NULL && lastAudioNode == NULL) {
        firstAudioNode = lastAudioNode = temp;
        return;
    } else {
        lastAudioNode->next = temp;
        lastAudioNode = temp;
    }

    audioQueueSize++;
}

MediaPlayerAudioQueueNode* mediaplayer_audio_queue_pop () {
    MediaPlayerAudioQueueNode* temp = firstAudioNode;

    firstAudioNode = temp->next;
    audioQueueSize--;

    return temp;
}

int mediaplayer_audio_queue_get_size () {
    return audioQueueSize;
}