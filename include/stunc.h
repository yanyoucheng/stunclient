#ifndef STUNC_H
#define STUNC_H
#define   BindingRequest	0x0001
#define   BindingResponse	0x0101
#define   BindingErrorResponse	0x0111
#define   SharedSecretRequest		0x0002
#define   SharedSecretResponse	0x0102
#define   SharedSecretErrorResponse	0x0112

#define   MAPPED_ADDRESS		0x0001
#define   RESPONSE_ADDRESS		0x0002
#define   CHANGE_REQUEST		0x0003
#define   SOURCE_ADDRESS		0x0004
#define   CHANGED_ADDRESS		0x0005
#define   USERNAME				0x0006
#define   PASSWORD				0x0007
#define   MESSAGE_INTEGRITY 	0x0008
#define   ERROR_CODE			0x0009
#define   UNKNOWN_ATTRIBUTES	0x000a
#define   REFLECTED_FROM		0x000b
#define   XOR_MAPPED_ADDRESS    0x8020
#define   SERVER				0x8022

struct Attributes{
	short int attribute_type;
	short int len;
	short int ipChanged;
	short int portChanged;
};
struct Ma_Attributes{
	short int attribute_type;
	short int len;
	short int pf;
	short int port;
	char addr[4];
};
struct Sa_Attributes{
	short int attribute_type;
	short int len;
	short int pf;
	short int port;
	char addr[4];
};
struct Ca_Attributes{
	short int attribute_type;
	short int len;
	short int pf;
	short int port;
	char addr[4];
};
struct Xm_Attributes{
	short int attribute_type;
	short int len;
	short int pf;
	short int xport;
	char xaddr[4];
};
struct Sr_Attributes{
	short int attribute_type;
	short int len;
	char sv[20];
};
struct StunRequest{
	short int message_type;
	short int len;
	char id[16];
	struct Attributes attr;
};
struct StunResponse{
	short int message_type;
	short int len;
	char id[16];
	struct Attributes attr;
};
void parse_message(char *buf, int len);
void print_ip(char *ip);
#undef STUNC_H
#endif
