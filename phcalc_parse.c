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

void phcalc_release_tokens(ttoken *tokens, int count);
void phcalc_release_stree(tsnode *stree);

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
		phcalc_release_tokens(tokens,len);
		phcalc_parse_printerror(stderr,&err);
		return 0;
	}
	expr = phcalc_parse_compile_expr(stree,&err);
	phcalc_release_tokens(tokens,len);
	phcalc_release_stree(stree);
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
	src = NEWS(char,len+1);
	if(src==0) return 0;
	len = fread(src,1,len,fd);
	src[len] = 0;
	if( !phcalc_parse_lexic(src,&tokens,&len,&err) ){
		phcalc_parse_printerror(stderr,&err);
		return 0;
	}
	free(src);
	if( !phcalc_parse_syntax(tokens,len,&stree,&err) ){
		phcalc_release_tokens(tokens,len);
		phcalc_parse_printerror(stderr,&err);
		return 0;
	}
	inst = phcalc_parse_compile_inst(stree,&err);
	phcalc_release_tokens(tokens,len);
	phcalc_release_stree(stree);
	if( inst==0 ){
		phcalc_parse_printerror(stderr,&err);
		return 0;
	}
	return inst;
}

void phcalc_release_tokens(ttoken *tokens, int count) {
	int i;
	for(i=0; i<count; i++){
		if(tokens[i].str!=0)
			FREE(tokens[i].str);
	}
	FREE(tokens);
}

void phcalc_release_stree(tsnode *stree) {
	int i;
	for(i=0; i<stree->nodes_len; i++){
		phcalc_release_stree(stree->nodes[i]);
	}
	if(stree->nodes_len>0)
		FREE(stree->nodes);
	FREE(stree);
}
