#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "phcalc.h"
#include "phcalc_in.h"

const char *ptrimc(const char *str, int *len);

phcalc_toper *phcalc_parse_rec(phcalc_expr expr, const char *str, int len);
int phcalc_parse_sign(const char *str, phcalc_opertype *oper);
int phcalc_parse_number(const char *str, int len, phcalc_num *num);

phcalc_expr phcalc_parse(phcalc_inst inst, const char *str){
	phcalc_expr expr = (phcalc_expr) malloc(sizeof(struct _phcalc_expr));
	expr->names = 0;
	expr->roper = phcalc_parse_rec(expr,str,strlen(str));
	return expr;
}

phcalc_toper *phcalc_parse_rec(phcalc_expr expr, const char *str, int len) {
	phcalc_toper *oper = (phcalc_toper*) malloc(sizeof(phcalc_toper));
	int depth = 0;
	int i;
	int oper_priority = 10;
	int oper_pos = -1;
	phcalc_opertype oper_type;
	int oper_len = 0;
	int cnt_depth1 = 0;		// counts brackets with depth==1
	int cnt_commas = 0;
	int cnt_semicolons = 0;
	int first_brk = -1;
	str = ptrimc(str,&len);
	for(i=0; i<len; i++) {
		switch(str[i]){
		case '(':
		case '[':
		case '{':
			depth ++;
			if(depth==1){
				cnt_depth1 ++;
				if(cnt_depth1==1)
					first_brk = i;
			}
			break;
		case ')':
		case ']':
		case '}':
			depth --;
			break;
		case ',':
			if(depth==1)
				cnt_commas ++;
			break;
		case ';':
			if(depth==0)
				cnt_semicolons ++;
			break;
		default:
			if(depth==0){
				phcalc_opertype oper;
				int olen;
				if( olen = phcalc_parse_sign(str+i,&oper) ){
					if( phcalc_getoperpriority(oper) <= oper_priority ){
						oper_pos		= i;
						oper_type		= oper;
						oper_len		= olen;
						oper_priority	= phcalc_getoperpriority(oper);
					}
					i += olen - 1;
				}
			}
		}
	}
	if(oper_pos!=-1)
	switch(oper_type){
	case PHC_OPER_ADD:
	case PHC_OPER_SUB:
	case PHC_OPER_MUL:
	case PHC_OPER_DIV:
	case PHC_OPER_DEF:
		oper->type		= oper_type;
		oper->id		= -1;
		oper->nargs		= 2;
		oper->args		= (phcalc_toper**) malloc( sizeof(phcalc_toper*)*oper->nargs );
		oper->args[0]	= phcalc_parse_rec(expr,str,oper_pos);
		oper->args[1]	= phcalc_parse_rec(expr,str+oper_pos+oper_len,len-oper_pos-oper_len);
		return oper;
	}
	if(cnt_depth1==0){
		int id;
		char *name = (char*) malloc(sizeof(char)*(len+1));
		phcalc_num num;
		if( phcalc_parse_number(str,len,&num) ){
			oper->type		= PHC_OPER_NUM;
			oper->id		= -1;
			oper->nargs		= 0;
			oper->num		= num;
			return oper;
		}
		memcpy(name,str,len);
		name[len] = 0;
		id = phcalc_expr_allocname(expr,name);
		free(name);
		oper->type	= PHC_OPER_VAR;
		oper->id	= id;
		oper->nargs	= 0;
		return oper;
	}
	if(cnt_depth1==1 && first_brk==0){
		return phcalc_parse_rec(expr,str+1,len-2);
	}
	free(oper);
	return 0;
}

int phcalc_parse_sign(const char *str, phcalc_opertype *oper) {
	switch(str[0]){
		case '+':		*oper = PHC_OPER_ADD;	return 1;
		case '-':		*oper = PHC_OPER_SUB;	return 1;
		case '*':		*oper = PHC_OPER_MUL;	return 1;
		case '/':		*oper = PHC_OPER_DIV;	return 1;
		case ':':
			if( str[1] == '=' ){
				*oper = PHC_OPER_DEF;
				return 2;
			} else
				return 0;
	}
	return 0;
}

int phcalc_parse_number(const char *str, int len, phcalc_num *num) {
	int i;
	int marker = 0;
	char *buf;
	for(i=0; i<len; i++){
		if( str[i]=='\'' || str[i]=='~' ){
			marker = i;
			continue;
		}
		if( !(str[i]>='0'&&str[i]<='9') && str[i]!='.' )
			return 0;
	}
	buf = (char*) malloc(len+1);
	memcpy(buf,str,len);
	buf[len] = 0;
	sscanf(buf,"%lf",&num->value);
	if(marker!=0){
		sscanf(buf+marker+1,"%lf",&num->error);
		if(buf[marker]=='~')
			num->error *= num->value;
	} else
		num->error = 0.0;
	free(buf);
	return 1;
}

const char *ptrimc(const char *str, int *len) {
	while( *len > 0 && isspace(str[0]) ){
		str ++;
		(*len) --;
	}
	while( *len > 0 && isspace(str[*len-1]) ){
		(*len) --;
	}
	return str;
}