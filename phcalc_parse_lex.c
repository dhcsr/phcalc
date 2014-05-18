/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 *
 * Lexic analysator source file (phcalc_parse.c)
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

#include "dynarr.h"
#define LEX_MAXNAMELEN	256

int phcalc_parse_lexic(const char *src, ttoken **tokens, int *len, tparseerr *err) {
	int line = 0, pos = -1;
	ttokentype state = TOKEN_NEW;
	int src_pos = 0;

	char name[LEX_MAXNAMELEN];
	int name_p = 0;
	int token_pos = -1;
	int number_exp = 0;

	dynarr_create((void**)tokens,sizeof(ttoken));

	for(;;){
		char c = src[src_pos++];
		int fnext = 0;
		pos ++;
		while(!fnext){
			fnext = 1;
			switch(state){
			case TOKEN_NEW:
				token_pos = pos;
				name_p = 0;
				switch(c){
				case '+':
					dynarr_add((void**)tokens,new_token(TOKEN_PLUS,0,line,pos));
					break;
				case '-':
					dynarr_add((void**)tokens,new_token(TOKEN_MINUS,0,line,pos));
					break;
				case '*':
					dynarr_add((void**)tokens,new_token(TOKEN_ASTER,0,line,pos));
					break;
				case '/':
					dynarr_add((void**)tokens,new_token(TOKEN_SLASH,0,line,pos));
					break;
				case ',':
					dynarr_add((void**)tokens,new_token(TOKEN_COMMA,0,line,pos));
					break;
				case ';':
					dynarr_add((void**)tokens,new_token(TOKEN_ENDLN,0,line,pos));
					break;
				case '=':
					dynarr_add((void**)tokens,new_token(TOKEN_ASSIGN,0,line,pos));
					break;
				case '(':
					dynarr_add((void**)tokens,new_token(TOKEN_BRPO,0,line,pos));
					break;
				case ')':
					dynarr_add((void**)tokens,new_token(TOKEN_BRPC,0,line,pos));
					break;
				case '[':
					dynarr_add((void**)tokens,new_token(TOKEN_BRSO,0,line,pos));
					break;
				case ']':
					dynarr_add((void**)tokens,new_token(TOKEN_BRSC,0,line,pos));
					break;
				case '{':
					dynarr_add((void**)tokens,new_token(TOKEN_BRCO,0,line,pos));
					break;
				case '}':
					dynarr_add((void**)tokens,new_token(TOKEN_BRCC,0,line,pos));
					break;
				default:
					if( (c>='a'&&c<='z') || (c>='A'&&c<='Z') || c=='_' ){
						state = TOKEN_NAME;
						fnext = 0;
					} else if( isspace(c) ){
						state = TOKEN_SPACE;
						fnext = 0;
					} else if( (c>='0'&&c<='9') ){
						state = TOKEN_NUMBER;
						fnext = 0;
					} else if(c=='\0') {
					} else {
						char buf[2] = { c, 0 };
						phcalc_parse_newerror(err,line,pos,0,"Unknown symbol",buf);
						*len = dynarr_desable((void**)tokens);
						return 0;
					}
				}
				break;
			case TOKEN_SPACE:
				if( c=='\n' ){
					line ++;
					pos = -1;
				}
				if( !isspace(c) ){
					state = TOKEN_NEW;
					fnext = 0;
				}
				break;
			case TOKEN_NAME:
				if( (c>='a'&&c<='z') || (c>='A'&&c<='Z') || (c>='0'&&c<='9') || c=='_' ){
					name[name_p++] = c;
				} else {
					name[name_p] = 0;
					dynarr_add((void**)tokens,new_token(TOKEN_NAME,name,line,token_pos));
					state = TOKEN_NEW;
					fnext = 0;
				}
				break;
			case TOKEN_NUMBER:
				if( (c>='0'&&c<='9') || c=='.' || c=='~' || c=='\'' || ( number_exp&&( c=='-' || c=='+' ) ) ){
					number_exp = 0;
					name[name_p++] = c;
				} else if( c=='e' ){
					number_exp = 1;
					name[name_p++] = c;
				} else {
					ttoken token = new_token(TOKEN_NUMBER,name,line,token_pos);
					name[name_p] = 0;
					if( !phcalc_parse_number(name,&token.num) ){
						phcalc_parse_newerror(err,line,pos,0,"Incorrect number",name);
						*len = dynarr_desable((void**)tokens);
						return 0;
					}
					dynarr_add((void**)tokens,token);
					state = TOKEN_NEW;
					fnext = 0;
				}
				break;
			}
		}
		if(c==0)
			break;
	}
	dynarr_add((void**)tokens,new_token(TOKEN_EOF,0,line,pos));
	*len = dynarr_desable((void**)tokens);
	return 1;
}

int phcalc_parse_number_int(const char *str, int *x) {
	int num = 0;
	int sign = 1;
	int pos = 1;
	if(str[0]=='-')
		sign = -1;
	else if(str[0]=='+')
		sign = 1;
	else
		pos --;
	while( str[pos]>='0' && str[pos]<='9' ){
		num = num*10 + str[pos] - '0';
		pos ++;
	}
	*x = num * sign;
	return pos;
}


int phcalc_parse_number_real(const char *str, double *x) {
	int integer = 0;
	int frac = 0;
	int frac_digs = 0;
	double devisor = 1.0;
	int pos = 0;
	int len = phcalc_parse_number_int(str,&integer);
	if(len==0)
		return 0;
	else
		pos += len;
	if(str[pos]!='.'){
		*x = (double) integer;
		return pos;
	}
	pos ++;
	frac_digs = phcalc_parse_number_int(str+pos,&frac);
	if(len==0)
		return 0;
	else
		pos += frac_digs;
	for(;frac_digs>0;frac_digs--)
		devisor *= 0.1;
	*x = integer + frac*devisor;
	return pos;
}

int phcalc_parse_number(const char *str, phcalc_num *num) {
	int len, pos = 0;
	num->error = 0.0;
	len = phcalc_parse_number_real(str,&num->value);
	if(len==0)
		return 0;
	else
		pos += len;
	if(str[pos]=='\''){
		pos ++;
		len = phcalc_parse_number_real(str+pos,&num->error);
		if(len==0)
			return 0;
		else
			pos += len;
	}
	if(str[pos]=='~'){
		double relative;
		pos ++;
		len = phcalc_parse_number_real(str+pos,&relative);
		if(len==0)
			return 0;
		else
			pos += len;
		num->error = num->value * relative;
	}
	if(str[pos]=='e'){
		int exp;
		double mutiplier = 1.0;
		pos ++;
		len = phcalc_parse_number_int(str+pos,&exp);
		if(len==0)
			return 0;
		else
			pos += len;
		if(exp>0){
			for(;exp>0;exp--)
				mutiplier *= 10.0;
		} else if(exp<0){
			for(;exp<0;exp++)
				mutiplier *= 0.1;
		}
		num->value *= mutiplier;
		num->error *= mutiplier;
	}
	return pos;
}

ttoken new_token(ttokentype type, const char *name, int line, int pos) {
	ttoken token;
	token.type = type;
	token.line = line;
	token.pos  = pos;
	if(name==0)
		token.str = 0;
	else
		token.str = _strdup(name);
	return token;
}

const char *gettokenname(ttokentype token) {
	switch(token){
	case TOKEN_NAME:			return "NAME";
	case TOKEN_NUMBER:			return "NUMBER";
	case TOKEN_PLUS:			return "+";
	case TOKEN_MINUS:			return "-";
	case TOKEN_ASTER:			return "*";
	case TOKEN_SLASH:			return "/";
	case TOKEN_BRPO:			return "(";
	case TOKEN_BRPC:			return ")";
	case TOKEN_BRCO:			return "{";
	case TOKEN_BRCC:			return "}";
	case TOKEN_BRSO:			return "[";
	case TOKEN_BRSC:			return "]";
	case TOKEN_EOF:				return "EOF";
	default:					return "";
	}
}