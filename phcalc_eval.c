#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "phcalc.h"
#include "phcalc_in.h"

//int phcalc_eval_args(phcalc_inst inst, phcalc_expr expr, 
phcalc_num phcalc_execoper_s2(phcalc_opertype opertype, phcalc_num arg1, phcalc_num arg2);

int phcalc_eval(phcalc_inst inst, phcalc_expr expr, phcalc_obj *res) {
	return phcalc_execoper(inst,expr,expr->roper,res);
}

int phcalc_execoper(phcalc_inst inst, phcalc_expr expr, phcalc_toper *oper, phcalc_obj *res) {
	switch(oper->type){
	case PHC_OPER_NUM: {
		res->type = PHC_OBJ_NUM;
		res->ref.num = (phcalc_num*) malloc( sizeof(phcalc_num) );
		*res->ref.num = oper->num;
		return 1; }
	case PHC_OPER_VAR: {
		//int type = phcalc_gettype(inst,expr->names[toper->id]);
		phcalc_expr def = phcalc_getdef(inst,expr->names[oper->id]);
		if(def==0)
			return 0;
		if(def->roper->args[0]->type!=PHC_OPER_VAR)
			return 0;
		if( !phcalc_execoper(inst,expr,def->roper->args[1],res) )
			return 0;
		return 1; }
	case PHC_OPER_VCT: {
		int i;
		phcalc_obj *rargs = NEWS(phcalc_obj,oper->nargs);
		for(i=0; i<oper->nargs; i++){
			if( !phcalc_execoper(inst,expr,oper->args[i],rargs+i) )
				return 0;		// TODO: release rargs
		}
		res->type = PHC_OBJ_VECT;
		res->ref.vect = NEW(phcalc_vect);
		res->ref.vect->len	= oper->nargs;
		res->ref.vect->vect	= rargs;
		return 1; }
	case PHC_OPER_ADD:
	case PHC_OPER_SUB:
	case PHC_OPER_MUL:
	case PHC_OPER_DIV:
	case PHC_OPER_POW: {
		phcalc_obj rargs[2];
		if(oper->nargs!=2)
			return 0;
		if( !phcalc_execoper(inst,expr,oper->args[0],rargs+0) )
			return 0;
		if( !phcalc_execoper(inst,expr,oper->args[1],rargs+1) ){
			phcalc_obj_release(inst,rargs[0]);
			return 0;
		}
		if( rargs[0].type==PHC_OBJ_NUM && rargs[1].type==PHC_OBJ_NUM ){
			res->type = PHC_OBJ_NUM;
			res->ref.num = (phcalc_num*) malloc( sizeof(phcalc_num) );
			*res->ref.num = phcalc_execoper_s2(oper->type,*rargs[0].ref.num,*rargs[1].ref.num);
			return 1;
		}
		if( rargs[0].type==PHC_OBJ_VECT && rargs[1].type==PHC_OBJ_NUM ){
			phcalc_obj sw = rargs[0];
			rargs[0] = rargs[1];
			rargs[1] = sw;
		}
		if( rargs[0].type==PHC_OBJ_NUM && rargs[1].type==PHC_OBJ_VECT ){
			int len = rargs[1].ref.vect->len;
			int i;
			res->type = PHC_OBJ_VECT;
			res->ref.vect = NEW(phcalc_vect);
			res->ref.vect->len	= len;
			res->ref.vect->vect = NEWS(phcalc_obj,len);
			for(i=0; i<len; i++){
				phcalc_obj t;
				// TODO:
				res->ref.vect->vect[i] = t;
			}
			return 1;
		}
		phcalc_obj_release(inst,rargs[0]);
		phcalc_obj_release(inst,rargs[1]);
		return 0; }
	}
	return 0;
}

phcalc_num phcalc_execoper_s2(phcalc_opertype opertype, phcalc_num arg1, phcalc_num arg2) {
	switch(opertype){
	case PHC_OPER_ADD:
		return phcalc_add(arg1,arg2);
	case PHC_OPER_SUB:
		return phcalc_sub(arg1,arg2);
	case PHC_OPER_MUL:
		return phcalc_mul(arg1,arg2);
	case PHC_OPER_DIV:
		return phcalc_div(arg1,arg2);
	case PHC_OPER_POW:
		return phcalc_pow(arg1,arg2);
	}
	assert(1);
	return phcalc_add(arg1,arg2);	// ???
}
