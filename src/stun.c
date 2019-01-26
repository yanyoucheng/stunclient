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

#include "transid.h"
#include "sock.h"
#include "stunc.h"
int main()
{
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	set_timeout(sock);
	//set_nonblock(sock);
	int res = 0;
	int len = sizeof(struct sockaddr_in);
	char buf[1024];
	struct sockaddr_in la;
	struct sockaddr_in sv;
	get_sockaddr("stun.ekiga.net", "3478", AF_INET, &sv);
	struct StunRequest req;
	struct StunResponse rsp;
	res = connect(sock, &sv, len);
	if(res < 0) {
		perror("connect fail\n");
		return -1;
	}
	res = getsockname(sock, &la, &len);
	if(res < 0) {
		perror("get local ip fail\n");
		return -1;
	}
	printf("local_addr = %s\n", inet_ntoa(la.sin_addr));
	build_request(&req, IP_NOT_CHANGED, PORT_NOT_CHANGED);
	res = sendto(sock, &req, sizeof(struct StunRequest), 0, &sv, len);
	if(res < 0) {
		perror("send fail\n");
		return -1;
	}
	memset(buf, 0, sizeof(buf));
	res = recvfrom(sock, buf, sizeof(buf), 0, &sv, &len);
	printf("res=%d\n",res);
	if(res < 0) {
		perror("recv fail\n");
		return -1;
	}
	else if(res == 0)
		close(sock);
	else
		parse_message(buf, res);
	memcpy(&rsp, buf, res);
	if(strcmp(rsp.id, req.id) == 0) {
		if(strcmp(rsp.ma.addr, inet_ntoa(la.sin_addr)) == 0) {
			printf("Not behind NAT.\n");
		} else {
			printf("Behind NAT type:");
			build_request(&req, IP_CHANGED, PORT_CHANGED);
			res = sendto(sock, &req, sizeof(struct StunRequest), 0, &sv, len);
			if(res < 0) {
				perror("send fail\n");
				return -1;
			}
			memset(buf, 0, sizeof(buf));
			res = recvfrom(sock, buf, sizeof(buf), 0, &sv, &len);
			printf("res=%d\n",res);
			if(res > 0) {
				printf("Full Cone\n");
				return 0;
			}
			else if(res == 0) {
				close(sock);
				return 0;
			}

			build_request(&req, IP_NOT_CHANGED, PORT_CHANGED);
			res = sendto(sock, &req, sizeof(struct StunRequest), 0, &sv, len);
			if(res < 0) {
				perror("send fail\n");
				return -1;
			}
			memset(buf, 0, sizeof(buf));
			res = recvfrom(sock, buf, sizeof(buf), 0, &sv, &len);
			printf("res=%d\n",res);
			if(res > 0) {
				printf("Restricted Cone\n");
				//return 0;
			}
			else if(res == 0) {
				close(sock);
				return 0;
			}

			build_request(&req, IP_NOT_CHANGED, PORT_NOT_CHANGED);
			res = sendto(sock, &req, sizeof(struct StunRequest), 0, &sv, len);
			if(res < 0) {
				perror("send fail\n");
				return -1;
			}
			memset(buf, 0, sizeof(buf));
			res = recvfrom(sock, buf, sizeof(buf), 0, &sv, &len);
			printf("res=%d\n",res);
			if(res > 0) {
				printf("Port Restricted Cone\n");
				//return 0;
			}
			else if(res == 0) {
				close(sock);
				return 0;
			} else {
				printf("Symmetric Cone\n");
			}

		}
	} else {
		printf("transaction id not match.\n");
	}
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
			print_ip(xm.xaddr);
			buf += sizeof(struct Xm_Attributes);
			len -= sizeof(struct Xm_Attributes);
			break;
		case SERVER:
			printf("SERVER ADDRESS\n");
			struct Sr_Attributes sr;
			memcpy(&sr, buf, sizeof(struct Sr_Attributes));
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

int build_request(struct StunRequest* req, int ipChanged, int portChanged) {
	req->message_type = htons(BindingRequest);
	req->len = htons(8);
	get_transaction_id(req->id);
	req->attr.attribute_type = htons(CHANGE_REQUEST);
	req->attr.len = htons(4);
	req->attr.changed = htonl(ipChanged+portChanged);
}
