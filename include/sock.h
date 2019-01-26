/*
 * sock.h
 *
 *  Created on: 2019-1-25
 *      Author: oem
 */

#ifndef INCLUDE_SOCK_H_
#define INCLUDE_SOCK_H_

#include "stunc.h"
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

int set_timeout(int sock);
int set_nonblock(int sock);
int get_sockaddr(char* host, char* port, int af, struct sockaddr_in* sa);


#endif /* INCLUDE_SOCK_H_ */
