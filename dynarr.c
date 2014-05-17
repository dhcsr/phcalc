/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 * https://github.com/dhcsr/phcalc
 *
 * Dynamic array: source (dynarr.c)
 *
 **************************************/

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "dynarr.h"

#define SIGN	0xDABABB00

typedef struct _DYNARR_HDR {
	int sign;
	int length;
	int reserved;
	int elsize;
} DYNARR_HDR;

void dynarr_create(void **arr, int elsize) {
	DYNARR_HDR *hdr = (DYNARR_HDR*) malloc(sizeof(DYNARR_HDR)+elsize);
	hdr->sign     = SIGN;
	hdr->length   = 0;
	hdr->reserved = 1;
	hdr->elsize   = elsize;
	*arr = hdr + 1;
}

void dynarr_add(void **arr, ...) {
	DYNARR_HDR *hdr = (DYNARR_HDR*)(*arr) - 1;
	va_list ap;
	va_start(ap,arr);
	assert(hdr->sign==SIGN);
	if( hdr->length == hdr->reserved ){
		DYNARR_HDR *new_hdr;
		hdr->reserved *= 2;
		new_hdr = (DYNARR_HDR*) realloc(hdr,sizeof(DYNARR_HDR)+hdr->elsize*hdr->reserved);
		hdr = new_hdr;
		*arr = hdr + 1;
	}
	memcpy( (char*)(*arr) + hdr->length*hdr->elsize, ap, hdr->elsize );
	hdr->length ++;
}

int dynarr_desable(void **arr) {
	DYNARR_HDR *hdr = (DYNARR_HDR*)(*arr) - 1;
	int length;
	void *data;
	assert(hdr->sign==SIGN);
	length = hdr->length;
	data = malloc(hdr->elsize*length);
	memcpy(data,*arr,hdr->elsize*length);
	*arr = data;
	free(hdr);
	return length;
}