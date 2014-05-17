/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 * https://github.com/dhcsr/phcalc
 *
 * Parsing file functions source file (phcalc_parse.c)
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



phcalc_inst phcalc_parsefile(FILE *fd) {
	ttoken *tokens;
	tsnode *stree;
	int len;
	tparseerr err;
	phcalc_inst inst;
	if( !phcalc_parse_lexic(fd,&tokens,&len,&err) ){
		phcalc_parse_printerror(stderr,&err);
		return 0;
	}
	if( !phcalc_parse_syntax(tokens,len,&stree,&err) ){
		phcalc_parse_printerror(stderr,&err);
		return 0;
	}
	inst = phcalc_parse_compile_inst(stree,&err);
	if( inst==0 ){
		phcalc_parse_printerror(stderr,&err);
		return 0;
	}
	return inst;
}