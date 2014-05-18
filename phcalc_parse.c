/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 *
 * Parsing functions source file (phcalc_parse.c)
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
#include "phcalc_parse.h"

//const char *ptrimc(const char *str, int *len);

//phcalc_toper *phcalc_parse_rec(phcalc_expr expr, const char *str, int len);
//int phcalc_parse_sign(const char *str, phcalc_opertype *oper);
//int phcalc_parse_number(const char *str, int len, phcalc_num *num);

phcalc_expr phcalc_parse(const char *str){
	ttoken *tokens;
	tsnode *stree;
	int len;
	tparseerr err;
	phcalc_expr expr;
	if( !phcalc_parse_lexic(str,&tokens,&len,&err) ){
		phcalc_parse_printerror(stderr,&err);
		return 0;
	}
	if( !phcalc_parse_syntax_expr(tokens,len,&stree,&err) ){
		phcalc_parse_printerror(stderr,&err);
		return 0;
	}
	expr = phcalc_parse_compile_expr(stree,&err);
	if( expr==0 ){
		phcalc_parse_printerror(stderr,&err);
		return 0;
	}
	return expr;
}

phcalc_inst phcalc_parsefile(FILE *fd) {
	ttoken *tokens;
	tsnode *stree;
	int len;
	tparseerr err;
	phcalc_inst inst;
	char *src;
	fseek(fd,0,SEEK_END);
	len = ftell(fd);
	fseek(fd,0,SEEK_SET);
	src = (char*) malloc(len+1);
	len = fread(src,1,len,fd);
	src[len] = 0;
	if( !phcalc_parse_lexic(src,&tokens,&len,&err) ){
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