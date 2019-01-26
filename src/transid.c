/*
 * transid.c
 *
 *  Created on: 2019-1-26
 *      Author: oem
 */
#include "transid.h"

int get_transaction_id(char* id) {
	srand48((unsigned)time(NULL));
	long n = lrand48()%min;
	if(n < min)
		n += min;
	else if(n > max)
		n = n%min + min;
	printf("n=%ld\n", n);
	while(n > 0) {
		*id = n%10+'0';
		n = n/10;
		id += 1;
	}
	return 0;
}



