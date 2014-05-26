#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "phcalc.h"
#include "phcalc_in.h"

int phcalc_getopersign(char *str, phcalc_opertype opertype);
int phcalc_strexpr_2(phcalc_expr expr, phcalc_toper *oper, char *str, int len);

int phcalc_strexpr(phcalc_inst inst, phcalc_expr expr, char *str, int len) {
	return phcalc_strexpr_2(expr,expr->roper,str,len);
}

int phcalc_strexpr_2(phcalc_expr expr, phcalc_toper *oper, char *str, int len) {
	int p = 0;
	char buf[32];
	switch(oper->type){
	case PHC_OPER_VAR:
		strcpy(str+p,expr->names[oper->id]);
		p += strlen(expr->names[oper->id]);
		break;
	case PHC_OPER_NUM:
		sprintf_s(buf,32,"%0.2lf",oper->num.value);
		strcpy(str,buf);
		p += strlen(buf);
		break;
	case PHC_OPER_DEF:
	case PHC_OPER_ADD:
	case PHC_OPER_SUB:
	case PHC_OPER_MUL:
	case PHC_OPER_DIV:
	case PHC_OPER_POW:
		str[p++] = '(';
		p += phcalc_strexpr_2(expr,oper->args[0],str+p,len-p);
		str[p++] = ')';
		p += phcalc_getopersign(str+p,oper->type);
		str[p++] = '(';
		p += phcalc_strexpr_2(expr,oper->args[1],str+p,len-p);
		str[p++] = ')';

	}
	/*if(oper->nargs>0)
		str[p++] = '(';
	for(i=0; i<oper->nargs; i++){
		if(i>0)
			str[p++] = ',';
		p += phcalc_strexpr_2(expr,oper->args[i],str+p,len-p);
	}
	if(oper->nargs>0)
		str[p++] = ')';*/
	str[p] = 0;
	return p;
}

int phcalc_getopersign(char *str, phcalc_opertype opertype) {
	switch(opertype){
	case PHC_OPER_DEF:
		str[0] = ':';
		str[1] = '=';
		return 2;
	case PHC_OPER_ADD:
		str[0] = '+';
		return 1;
	case PHC_OPER_SUB:
		str[0] = '-';
		return 1;
	case PHC_OPER_MUL:
		str[0] = '*';
		return 1;
	case PHC_OPER_DIV:
		str[0] = '/';
		return 1;
	case PHC_OPER_POW:
		str[0] = '^';
		return 1;
	}
	return 0;
}

int phcalc_strobj(phcalc_obj *obj, char *str, int len) {
	if(obj->type==PHC_OBJ_NUM){
		char buf[128];
		char *buf2;
		int exp;
		double val, err, mul;
		if( obj->ref.num.value==0.0 )
			exp = 0;
		else
			exp = (int) floor( log10( dabs(obj->ref.num.value) ) );
		if( exp<3 && exp>-3 )
			exp = 0;
		mul = pow(10.0,exp);
		val = obj->ref.num.value / mul;
		err = obj->ref.num.error / mul;
		if( dcmp(err,0.0)==0 )
			sprintf(buf,"%f",(float)val);
		else
			sprintf(buf,"%f'%f",(float)val,(float)err);
		buf2 = buf + strlen(buf);
		if(exp!=0)
			sprintf(buf2,"e%d",exp);
		if( (int)strlen(buf) < len )
			strcpy(str,buf);
		else {
			memcpy(str,buf,len-1);
			str[len-1] = 0;
		}
		return 1;
	}
	if(obj->type==PHC_OBJ_VECT){
		int pos = 1;
		int i;
		str[0] = '{';
		for(i=0; i<obj->ref.vect.len; i++){
			if(i!=0)
				str[pos++] = ',';
			phcalc_strobj(&obj->ref.vect.data[i],str+pos,len-pos);
			pos = strlen(str);
		}
		str[pos] = '}';
		str[pos+1] = '\0';
		return 1;
	}
	str[0] = 0;
	return 0;
}