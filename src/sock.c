/*
 * sock.c
 *
 *  Created on: 2019-1-25
 *      Author: oem
 */
#include "sock.h"

int set_timeout(int sock) {
	struct timeval timeout={3,0};//3s
	setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));
	setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
	return 0;
}
int set_nonblock(int sock) {
	int cflags = fcntl(sock,F_GETFL,0);
	fcntl(sock,F_SETFL, cflags|O_NONBLOCK);
	return 0;
}
int get_sockaddr(char* host, char* port, int af, struct sockaddr_in* sa){
	struct addrinfo hints;
	struct addrinfo *res;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = af;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_socktype = SOCK_DGRAM;
	getaddrinfo(host, port, &hints, &res);
	char str[128] = {0};
	for(; res != NULL; res = res->ai_next) {
		if(af == AF_INET6)
		{
			struct sockaddr_in6* addr = (struct sockaddr_in6*)res->ai_addr;
			inet_ntop(AF_INET6, (void*)&addr->sin6_addr, str, 128);
			printf("ai_addr = %s\n", str);
			printf("ai_family = %d\n", res->ai_family);
			printf("ai_socktype = %d\n", res->ai_socktype);
			memcpy(sa, res->ai_addr, sizeof(struct sockaddr_in));
		}
		else
		{
			struct sockaddr_in* addr = (struct sockaddr_in*)res->ai_addr;
			printf("ai_addr = %s\n", inet_ntoa(addr->sin_addr));
			printf("ai_port = %d\n", ntohs(addr->sin_port));
			printf("ai_family = %d\n", res->ai_family);
			printf("ai_socktype = %d\n", res->ai_socktype);
			memcpy(sa, res->ai_addr, sizeof(struct sockaddr_in));
			//if(hints->ai_socktype == SOCK_DGRAM)
				//break;
		}
	}
	freeaddrinfo(res);
	return 0;
}


