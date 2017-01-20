#ifndef MEDIAPLAYER_GRABBER_HEADER
#define MEDIAPLAYER_GRABBER_HEADER

#include <stdbool.h>

void mediaplayer_start_grabber ();
void *mediaplayer_start_grabber_thread (void * args);
int mediaplayer_grabber_packet (bool isVideoPacket);

#endif