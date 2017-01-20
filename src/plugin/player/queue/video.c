#include <stdlib.h>

#include "video.h"
#include "../../debug.h"

MediaPlayerVideoQueueNode* firstVideoNode = NULL;
MediaPlayerVideoQueueNode* lastVideoNode = NULL;

int videoQueueSize = 0;

void mediaplayer_video_queue_enqueue (uint8_t *data) {
    MediaPlayerVideoQueueNode* temp = malloc(sizeof(MediaPlayerVideoQueueNode));

    temp->data = data;
    temp->next = NULL;
    temp->index = videoQueueSize; // Size is 0 based at this point

    /*
     * If nothing is in the queue yet the new node is both
     * the rear and front of the queue
     */
    if (firstVideoNode == NULL && lastVideoNode == NULL) {
        firstVideoNode = lastVideoNode = temp;
        return;
    } else {
        lastVideoNode->next = temp;
        lastVideoNode = temp;
    }

    videoQueueSize++;
}

MediaPlayerVideoQueueNode* mediaplayer_video_queue_pop () {
    MediaPlayerVideoQueueNode* temp = firstVideoNode;

    firstVideoNode = temp->next;
    videoQueueSize--;

    return temp;
}

MediaPlayerVideoQueueNode* mediaplayer_video_queue_first () {
    return firstVideoNode;
}

int mediaplayer_video_queue_get_size () {
    return videoQueueSize;
}