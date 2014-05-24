#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "phcalc.h"
#include "phcalc_in.h"

phcalc_num phcalc_execoper_s2(phcalc_opertype opertype, phcalc_num arg1, phcalc_num arg2);

int phcalc_execoper(phcalc_evalctx *ctx, phcalc_toper *oper, phcalc_obj *res);
int phcalc_execoper1(phcalc_evalctx *ctx, phcalc_toper *oper, phcalc_obj *args, phcalc_typedef *types, phcalc_obj *res);
int phcalc_execoper2(phcalc_evalctx *ctx, phcalc_toper *oper, phcalc_obj *args, phcalc_obj *res);

int phcalc_getargtypes(phcalc_evalctx *ctx, phcalc_toper *toper, phcalc_typedef *types);
int phcalc_comparetypes_map(phcalc_evalctx *ctx, phcalc_obj *arg, phcalc_typedef *type);

int phcalc_eval2(phcalc_inst inst, phcalc_expr expr, phcalc_toper *roper, phcalc_obj *res);

int phcalc_eval(phcalc_inst inst, phcalc_expr expr, phcalc_obj *res) {
	return phcalc_eval2(inst,expr,expr->roper,res);
}

int phcalc_eval2(phcalc_inst inst, phcalc_expr expr, phcalc_toper *roper, phcalc_obj *res) {
	int ret;
	phcalc_evalctx *ctx = NEW(phcalc_evalctx);
	ctx->inst = inst;
	ctx->expr = expr;
	ret = phcalc_execoper(ctx,roper,res);
	FREE(ctx);
	return ret;
}

int phcalc_execoper(phcalc_evalctx *ctx, phcalc_toper *oper, phcalc_obj *res) {
	int i;
	phcalc_typedef *types;
	phcalc_obj *args = NEWS(phcalc_obj,oper->nargs);
	for(i=0; i<oper->nargs; i++){
		if( !phcalc_execoper(ctx,oper->args[i],&args[i]) ){
			FREE(args);
			return 0;
		}
	}
	types = NEWS(phcalc_typedef,oper->nargs);
	phcalc_getargtypes(ctx,oper,types);
	phcalc_execoper1(ctx,oper,args,types,res);
	for(i=0; i<oper->nargs; i++)
		phcalc_release_obj(ctx->inst,&args[i]);
	FREE(args);
	FREE(types);
	return 1;
}

int phcalc_execoper1(phcalc_evalctx *ctx, phcalc_toper *oper, phcalc_obj *args, phcalc_typedef *types, phcalc_obj *res) {
	int i, j, count = -1, ret;
	phcalc_obj *rargs;
	int *amapping = NEWS(int,oper->nargs);
	for(i=0; i<oper->nargs; i++){
		amapping[i] = phcalc_comparetypes_map(ctx,&args[i],&types[i]);
		if(amapping[i]==-1){
			FREE(amapping);
			return 0;
		}
	}
	for(i=0; i<oper->nargs; i++){
		if(amapping[i]){
			int cnt = args[i].ref.vect.len;
			if(count==-1)
				count = cnt;
			else if(count!=cnt){
				FREE(amapping);
				return 0;
			}
		}
	}
	if(count==-1){
		ret = phcalc_execoper2(ctx,oper,args,res);
		FREE(amapping);
		return ret;
	}
	res->type = PHC_OBJ_VECT;
	res->ref.vect.len	= count;
	res->ref.vect.data	= NEWS(phcalc_obj,count);
	rargs = NEWS(phcalc_obj,oper->nargs);
	for(j=0; j<count; j++){
		for(i=0; i<oper->nargs; i++){
			if(amapping[i]){
				rargs[i] = args[i].ref.vect.data[j];
			} else
				rargs[i] = args[i];
		}
		if( !phcalc_execoper1(ctx,oper,rargs,types,&res->ref.vect.data[j]) ){
			FREE(amapping);
			FREE(rargs);
			return 0;
		}
	}
	FREE(amapping);
	return 1;
}

int phcalc_execoper2(phcalc_evalctx *ctx, phcalc_toper *oper, phcalc_obj *args, phcalc_obj *res) {
	int i;
	switch(oper->type){
	case PHC_OPER_NUM: {
		res->type = PHC_OBJ_NUM;
		res->ref.num = oper->num;
		return 1; }
	case PHC_OPER_VAR: {
		phcalc_expr def = phcalc_getdef(ctx->inst,ctx->expr->names[oper->id]);
		if(def==0){
			phcalc_num num;
			if( !phcalc_const(ctx->expr->names[oper->id],&num) )
				return 0;
			res->type		= PHC_OBJ_NUM;
			res->ref.num	= num;
			return 1;
		}
		if(def->roper->args[0]->type!=PHC_OPER_VAR)
			return 0;
		if( !phcalc_eval2(ctx->inst,def,def->roper->args[1],res) )
			return 0;
		return 1; }
	case PHC_OPER_VCT:
		res->type = PHC_OBJ_VECT;
		res->ref.vect.len	= oper->nargs;
		res->ref.vect.data	= NEWS(phcalc_obj,oper->nargs);
		for(i=0; i<oper->nargs; i++)
			res->ref.vect.data[i] = phcalc_clone_obj(ctx->inst,&args[i]);
		return 1;
	case PHC_OPER_ADD:
	case PHC_OPER_SUB:
	case PHC_OPER_MUL:
	case PHC_OPER_DIV:
	case PHC_OPER_POW:
		if(oper->nargs!=2)
			return 0;
		if( args[0].type==PHC_OBJ_NUM && args[1].type==PHC_OBJ_NUM ){
			res->type = PHC_OBJ_NUM;
			res->ref.num = phcalc_execoper_s2(oper->type,args[0].ref.num,args[1].ref.num);
			return 1;
		}
		break;
	}
	return 0;
}

int phcalc_getargtypes(phcalc_evalctx *ctx, phcalc_toper *oper, phcalc_typedef *types) {
	int i;
	for(i=0; i<oper->nargs; i++)
		types[i].type = PHC_OBJ_NUM;
	return 1;
}

int phcalc_comparetypes_map(phcalc_evalctx *ctx, phcalc_obj *arg, phcalc_typedef *type) {
	if( arg->type==PHC_OBJ_NUM && type->type==PHC_OBJ_NUM )
		return 0;
	if( arg->type==PHC_OBJ_VECT && type->type==PHC_OBJ_NUM )
		return 1;
	return -1;
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
	assert(0);
	return phcalc_add(arg1,arg2);	// ???
}
