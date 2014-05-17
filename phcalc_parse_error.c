/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 * https://github.com/dhcsr/phcalc
 *
 * Error handling for parser (phcalc_parse_error.c)
 *
 **************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "phcalc.h"
#include "phcalc_in.h"
#include "dynarr.h"
#include "phcalc_parse.h"

void phcalc_parse_newerror(tparseerr *err, int line, int pos, int code, const char *desc, const char *buf) {
	err->line	= line;
	err->pos	= pos;
	err->code	= code;
	err->desc	= desc;
	if(buf==0)
		err->buf[0] = 0;
	else
		strcpy(err->buf,buf);
}

void phcalc_parse_printerror(FILE *fd, tparseerr *err) {
	fprintf(fd,"Error: %s (%s) at line %d, %d\n",err->desc,err->buf,err->line+1,err->pos+1);
}