//network.h
//Module to spawn a separate thread which will listen the udp packet.
//based on the udp packet received, response some information.
#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "sorter.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

//init function
void udpListener_new(void);
//clean function
void udpListener_clean(void);

#endif
