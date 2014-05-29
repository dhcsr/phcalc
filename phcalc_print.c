#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "phcalc.h"
#include "phcalc_in.h"

int phcalc_strnum(phcalc_num num, char *str, int len);
int phcalc_getopersign(char *str, phcalc_opertype opertype);
int phcalc_strexpr_2(phcalc_expr expr, phcalc_toper *oper, int prev_prior, int reflect, char *str, int len);
int phcalc_strexpr_vect(phcalc_expr expr, phcalc_toper *oper, char *str, int len);

int phcalc_strexpr(phcalc_inst inst, phcalc_expr expr, char *str, int len) {
	return phcalc_strexpr_2(expr,expr->roper,0,0,str,len);
}

int phcalc_strexpr_2(phcalc_expr expr, phcalc_toper *oper, int prev_prior, int reflect, char *str, int len) {
	int p = 0;
	//char buf[32];
	switch(oper->type){
	case PHC_OPER_VAR:
		strcpy(str+p,expr->names[oper->id]);
		p += strlen(expr->names[oper->id]);
		break;
	case PHC_OPER_NUM:
		phcalc_strnum(oper->num,str+p,len-p);
		p += strlen(str+p);
		break;
	case PHC_OPER_VCT:
		str[p++] = '{';
		p += phcalc_strexpr_vect(expr,oper,str+p,len-p);
		str[p++] = '}';
		break;
	case PHC_OPER_FNC:
		strcpy(str+p,expr->names[oper->id]);
		p += strlen(expr->names[oper->id]);
		str[p++] = '(';
		p += phcalc_strexpr_vect(expr,oper,str+p,len-p);
		str[p++] = ')';
		break;
	case PHC_OPER_DEF:
	case PHC_OPER_ADD:
	case PHC_OPER_SUB:
	case PHC_OPER_MUL:
	case PHC_OPER_DIV:
	case PHC_OPER_POW: {
		int refl = 0;
		int prior = phcalc_getoperpriority(oper->type);
		if( oper->type==PHC_OPER_ADD || oper->type==PHC_OPER_MUL )
			refl = 1;
		if( prior < prev_prior || (prior==prev_prior && !reflect) )
			str[p++] = '(';
		p += phcalc_strexpr_2(expr,oper->args[0],prior,1,str+p,len-p);
		p += phcalc_getopersign(str+p,oper->type);
		p += phcalc_strexpr_2(expr,oper->args[1],prior,refl,str+p,len-p);
		if( prior < prev_prior || (prior==prev_prior && !reflect) )
			str[p++] = ')';
		break; }
	}
	str[p] = 0;
	return p;
}

int phcalc_strexpr_vect(phcalc_expr expr, phcalc_toper *oper, char *str, int len) {
	int i, p=0;
	for(i=0; i<oper->nargs; i++){
		if(i>0)
			str[p++] = ',';
		p += phcalc_strexpr_2(expr,oper->args[i],0,0,str+p,len-p);
	}
	return p;
}

int phcalc_getopersign(char *str, phcalc_opertype opertype) {
	switch(opertype){
	case PHC_OPER_DEF:
		str[0] = '=';
		return 1;
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

int phcalc_strnum(phcalc_num num, char *str, int len) {
	char buf[128];
	char *buf2;
	int exp;
	double val, err, mul;
		// retrieve exponent
	if( num.value==0.0 )
		exp = 0;
	else
		exp = (int) floor( log10( dabs(num.value) ) );
	if( exp<3 && exp>-3 )	// if less then 3, do not use exponent
		exp = 0;
	mul = pow(10.0,exp);	// multiplier = 10^exp
	val = num.value / mul;
	err = num.error / mul;
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

int phcalc_strobj(phcalc_obj *obj, char *str, int len) {
	if(obj->type==PHC_OBJ_NUM){
		return phcalc_strnum(obj->ref.num,str,len);
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
	if(obj->type==PHC_OBJ_EXPR){
		return phcalc_strexpr(0,obj->ref.expr,str,len);
	}
	str[0] = 0;
	return 0;
}