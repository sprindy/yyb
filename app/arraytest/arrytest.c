#include "stdio.h"
#include "stdlib.h"

#define BYTE_COUNT	128
#define WORD_COUNT  BYTE_COUNT >>2

int main(int argc, char *argv[])
{
	if(argc < BYTE_COUNT+1) {
		printf("byte count not enough\n");
		return -1;
	}
	printf("byte count is:%d\n", argc-1);

	uint8_t data[BYTE_COUNT] = {0};
	for(int i=0; i< BYTE_COUNT; i++) {
		data[i] = (uint8_t)strtoul(argv[i+1], NULL, 16);
	}
	uint32_t data32[WORD_COUNT] = {0};
	for(int i=0; i< WORD_COUNT; i++) {
		data32[i]=data[4*i]<<24 | data[4*i+1]<<16 | data[4*i+2]<<8 | data[4*i+3];
	}

	for(int i=31; i>=0; i--) {
		for(int j=0; j<WORD_COUNT; j++) {
			if(data32[j] & (0x1<<i))
				printf("*");
			else
				printf("-");
		}
		printf("\n");
	}

	return 0;
}
