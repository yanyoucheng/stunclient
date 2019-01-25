/*
 ============================================================================
 Name        : stun.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "stunc.h"
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/tcp.h>


int main()
{
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	int flag = 1;
	struct addrinfo *hints = 0;
	hints = (struct addrinfo*)malloc(sizeof(*hints));
	hints->ai_socktype = SOCK_STREAM;
	getaddrinfo("stun.ekiga.net", "3478", NULL, &hints);
	char str[128] = {0};
	for(; hints != NULL; hints = hints->ai_next) {
		if(hints->ai_family ==  AF_INET6)
		{
			struct sockaddr_in6* addr = (struct sockaddr_in6*)hints->ai_addr;
			inet_ntop(AF_INET6, (void*)&addr->sin6_addr, str, 128);
			printf("ai_addr = %s\n", str);
			printf("ai_family = %d\n", hints->ai_family);
			printf("ai_socktype = %d\n", hints->ai_socktype);
		}
		else
		{
			struct sockaddr_in* addr = (struct sockaddr_in*)hints->ai_addr;
			printf("ai_addr = %s\n", inet_ntoa(addr->sin_addr));
			printf("ai_port = %d\n", ntohs(addr->sin_port));
			printf("ai_family = %d\n", hints->ai_family);
			printf("ai_socktype = %d\n", hints->ai_socktype);
			if(hints->ai_socktype == SOCK_STREAM)
				break;
		}
	}
	struct StunRequest req;
	req.message_type = htons(BindingRequest);
	req.len = htons(8);
	strcpy(req.id,"0123456789012345");
	req.attr.attribute_type = htons(CHANGE_REQUEST);
	req.attr.len = htons(4);
	req.attr.ipChanged = htons(0);
	req.attr.portChanged = htons(0);

	int res = sendto(sock, &req, sizeof(struct StunRequest), 0, hints->ai_addr, hints->ai_addrlen);
	printf("res=%d\n",res);
	if(res < 0)
		return -1;
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	res = recvfrom(sock, buf, sizeof(buf), 0, hints->ai_addr, &hints->ai_addrlen);
	printf("res=%d\n",res);
	if(res < 0)
		return -1;
	else if(res == 0)
		close(sock);
	else
		parse_message(buf, res);
	return 0;
}

void parse_message(char *buf, int len){
	printf("len=%d",len);
	short int message_type = 0;
	printf("\nIncomming message type: ");
	memcpy(&message_type, buf, 2);
	switch(ntohs(message_type)){
	case BindingRequest:
		break;
	case BindingResponse:
		printf("Binding Response.\n");
		break;
	case BindingErrorResponse:
		break;
	case SharedSecretRequest:
		break;
	case SharedSecretResponse:
		break;
	case SharedSecretErrorResponse:
		break;
	default:
		break;
	}
	buf += 20;
	len -= 20;

	while(len > 0){
		printf("len = %d\n", len);
		sleep(5);
		printf("attribute type: ");
		short int attribute_type = 0;
		memcpy(&attribute_type, buf, 2);
		switch(ntohs(attribute_type)){
		case MAPPED_ADDRESS:
			printf("MAPPED ADDRESS\n");
			struct Ma_Attributes ma;
			memcpy(&ma, buf, sizeof(struct Ma_Attributes));
			printf("pf:%d\n", ntohs(ma.pf));
			printf("port:%d\n", ntohs(ma.port));
			print_ip(ma.addr);
			buf += sizeof(struct Ma_Attributes);
			len -= sizeof(struct Ma_Attributes);
			break;
		case RESPONSE_ADDRESS:
			break;
		case CHANGE_REQUEST:
			break;
		case SOURCE_ADDRESS:
			printf("SOURCE ADDRESS\n");
			struct Sa_Attributes sa;
			memcpy(&sa, buf, sizeof(struct Sa_Attributes));
			printf("pf:%d\n", ntohs(sa.pf));
			printf("port:%d\n", ntohs(sa.port));
			printf("addr:");
			print_ip(sa.addr);
			buf += sizeof(struct Sa_Attributes);
			len -= sizeof(struct Sa_Attributes);
			break;
		case CHANGED_ADDRESS:
			printf("CHANGED ADDRESS\n");
			struct Ca_Attributes ca;
			memcpy(&ca, buf, sizeof(struct Ca_Attributes));
			printf("pf:%d\n", ntohs(ca.pf));
			printf("port:%d\n", ntohs(ca.port));
			print_ip(ca.addr);
			buf += sizeof(struct Ca_Attributes);
			len -= sizeof(struct Ca_Attributes);
			break;
		case USERNAME:
			break;
		case PASSWORD:
			break;
		case MESSAGE_INTEGRITY:
			break;
		case ERROR_CODE:
			break;
		case UNKNOWN_ATTRIBUTES:
			break;
		case REFLECTED_FROM:
			break;
		case XOR_MAPPED_ADDRESS:
			printf("XOR MAPPED ADDRESS\n");
			struct Xm_Attributes xm;
			memcpy(&xm, buf, sizeof(struct Xm_Attributes));
			printf("pf:%d\n", ntohs(xm.pf));
			printf("xport:%d\n", ntohs(xm.xport));
			printf("port:%d\n", ntohs(xm.port));
			print_ip(xm.xaddr);
			print_ip(xm.addr);
			buf += sizeof(struct Xm_Attributes);
			len -= sizeof(struct Xm_Attributes);
			break;
		case SERVER:
			printf("SOURCE ADDRESS\n");
			struct Sr_Attributes sr;
			memcpy(&sa, buf, sizeof(struct Sr_Attributes));
			printf("version:%s\n", sr.sv);
			buf += sizeof(struct Sr_Attributes);
			len -= sizeof(struct Sr_Attributes);
			break;
		default:
			break;
		}

	}
}
void print_ip(char *ip){
	int num = 0;
	int count = 4;
	while(count > 0) {
		num = *ip;
		num &= 0x000000FF;
		printf("%d", num);
		ip += 1;
		count--;
		if(count > 0)
			printf(".");
	}
	printf("\n");
}
