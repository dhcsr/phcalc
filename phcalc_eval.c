#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "phcalc.h"
#include "phcalc_in.h"

#define MAX_REC_DEPTH		512

phcalc_num phcalc_execoper_s2(phcalc_opertype opertype, phcalc_num arg1, phcalc_num arg2);

int phcalc_execoper(phcalc_evalctx *ctx, phcalc_toper *oper, phcalc_obj *res);
int phcalc_execoper1(phcalc_evalctx *ctx, phcalc_toper *oper, phcalc_obj *args, phcalc_typedef *types, phcalc_obj *res);
int phcalc_execoper2(phcalc_evalctx *ctx, phcalc_toper *oper, phcalc_obj *args, phcalc_obj *res);

int phcalc_getargtypes(phcalc_evalctx *ctx, phcalc_toper *toper, phcalc_typedef *types);
int phcalc_comparetypes_map(phcalc_evalctx *ctx, phcalc_obj *arg, phcalc_typedef *type);

int phcalc_eval2(phcalc_evalctx *ctx, phcalc_expr expr, phcalc_toper *roper, phcalc_obj *res);
int phcalc_eval_func(phcalc_evalctx *ctx, phcalc_expr expr, phcalc_obj *arg, phcalc_obj *res);

int phcalc_vector_depth(phcalc_vect vect);

/*
	Evaluate
	Interface function
	called from the outside of phcalc_library
	returns resulat to `res`
*/
int phcalc_eval(phcalc_inst inst, phcalc_expr expr, phcalc_obj *res) {
	phcalc_evalctx *ctx = NEW(phcalc_evalctx);
	int ret;
	ctx->prev = 0;
	ctx->inst = inst;
	ctx->expr = 0;
	ctx->rec_depth = 0;
	ret = phcalc_eval2(ctx,expr,expr->roper,res);
	if(!ret)
		phcalc_eval_printerror(stderr,&ctx->err);
	FREE(ctx);
	return ret;
}

/*
	Evaluate expression `expr` begin with operation `roper`
	create new eval. context
	called to calculate variable or function
*/
int phcalc_eval2(phcalc_evalctx *ctx, phcalc_expr expr, phcalc_toper *roper, phcalc_obj *res) {
	int ret;
	phcalc_evalctx *nctx = NEW(phcalc_evalctx);
	nctx->prev = ctx;
	nctx->inst = ctx->inst;
	nctx->expr = expr;
	nctx->rec_depth = ctx->rec_depth + 1;
	if( nctx->rec_depth > MAX_REC_DEPTH ){
		phcalc_eval_newerror(&ctx->err,0,"Recursion is too deep",0);
		FREE(nctx);
		return 0;
	}
	ret = phcalc_execoper(nctx,roper,res);
	if(!ret)
		ctx->err = nctx->err;
	FREE(nctx);
	return ret;
}

/*
	Execute operation
	Step 1.
*/
int phcalc_execoper(phcalc_evalctx *ctx, phcalc_toper *oper, phcalc_obj *res) {
	int i;
	int ret;
	phcalc_typedef *types = 0;
	phcalc_obj *args = 0;
	if( oper->nargs > 0 )
		args = NEWS(phcalc_obj,oper->nargs);
	for(i=0; i<oper->nargs; i++){
		if( !phcalc_execoper(ctx,oper->args[i],&args[i]) ){
			if( oper->nargs > 0 )
				FREE(args);
			return 0;
		}
	}
	if( oper->nargs > 0 )
		types = NEWS(phcalc_typedef,oper->nargs);
	phcalc_getargtypes(ctx,oper,types);
	ret = phcalc_execoper1(ctx,oper,args,types,res);
	for(i=0; i<oper->nargs; i++)
		phcalc_release_obj(ctx->inst,&args[i]);
	if( oper->nargs > 0 ){
		FREE(args);
		FREE(types);
	}
	return ret;
}

/*
	Execute operation
	Step 2.
*/
int phcalc_execoper1(phcalc_evalctx *ctx, phcalc_toper *oper, phcalc_obj *args, phcalc_typedef *types, phcalc_obj *res) {
	int i, j, count = -1, ret;
	phcalc_obj *rargs = 0;
	int *amapping = 0;
	if( oper->nargs > 0 )
		amapping = NEWS(int,oper->nargs);
	for(i=0; i<oper->nargs; i++){
		amapping[i] = phcalc_comparetypes_map(ctx,&args[i],&types[i]);
		if(amapping[i]==-1){
			phcalc_eval_newerror(&ctx->err,0,"Unable to determine a type of argument for operation",0);
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
				phcalc_eval_newerror(&ctx->err,0,"Incompatible vector to process",0);
				if( oper->nargs > 0 )
					FREE(amapping);
				return 0;
			}
		}
	}
	if(count==-1){
		ret = phcalc_execoper2(ctx,oper,args,res);
		if( oper->nargs > 0 )
			FREE(amapping);
		return ret;
	}
	res->type = PHC_OBJ_VECT;
	res->ref.vect.len	= count;
	res->ref.vect.data	= NEWS(phcalc_obj,count);
	if( oper->nargs > 0 )
		rargs = NEWS(phcalc_obj,oper->nargs);
	for(j=0; j<count; j++){
		for(i=0; i<oper->nargs; i++){
			if(amapping[i]){
				rargs[i] = args[i].ref.vect.data[j];
			} else
				rargs[i] = args[i];
		}
		if( !phcalc_execoper1(ctx,oper,rargs,types,&res->ref.vect.data[j]) ){
			if( oper->nargs > 0 ){
				FREE(amapping);
				FREE(rargs);
			}
			return 0;
		}
	}
	if( oper->nargs > 0 ){
		FREE(amapping);
		FREE(rargs);
	}
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
		phcalc_obj *def;
		assert(oper->id!=-1);
		def = phcalc_getdef(ctx->inst,ctx->expr->names[oper->id]);
		if(def==0){
			phcalc_num num;
			if( !phcalc_const(ctx->expr->names[oper->id],&num) ){
				phcalc_eval_newerror(&ctx->err,0,"Variable is not found",ctx->expr->names[oper->id]);
				return 0;
			}
			res->type		= PHC_OBJ_NUM;
			res->ref.num	= num;
			return 1;
		}
		if( def->type==PHC_OBJ_NUM || def->type==PHC_OBJ_VECT ){
			*res = phcalc_clone_obj(ctx->inst,def);
			return 1;
		} else if( def->type==PHC_OBJ_EXPR ){
			phcalc_expr dexpr = def->ref.expr;
			if(dexpr->roper->args[0]->type!=PHC_OPER_VAR){
				phcalc_eval_newerror(&ctx->err,0,"Defined object isn't a variable",ctx->expr->names[oper->id]);
				return 0;
			}
			if( !phcalc_eval2(ctx,dexpr,dexpr->roper->args[1],res) )
				return 0;
			return 1; }
		assert(0);
		return 0; }
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
		if(oper->nargs!=2){
			phcalc_eval_newerror(&ctx->err,0,"Incorrect argument number",0);
			return 0;
		}
		if( args[0].type==PHC_OBJ_NUM && args[1].type==PHC_OBJ_NUM ){
			res->type = PHC_OBJ_NUM;
			res->ref.num = phcalc_execoper_s2(oper->type,args[0].ref.num,args[1].ref.num);
			return 1;
		}
		break;
	case PHC_OPER_FNC: {
		phcalc_obj *def = phcalc_getdef(ctx->inst,ctx->expr->names[oper->id]);
		if(def==0){
			if( !phcalc_mfunc(ctx,ctx->expr->names[oper->id],args,oper->nargs,res) ){
				phcalc_eval_newerror(&ctx->err,0,"Function is not found",ctx->expr->names[oper->id]);
				return 0;
			}
			return 1;
		}
		if( def->type==PHC_OBJ_NUM || def->type==PHC_OBJ_VECT ){
			*res = phcalc_clone_obj(ctx->inst,def);
			return 1;
		} else if( def->type==PHC_OBJ_EXPR ){
			phcalc_expr dexpr = def->ref.expr;
			if(dexpr->roper->args[0]->type!=PHC_OPER_FNC){
				phcalc_eval_newerror(&ctx->err,0,"Defined object isn't a function",ctx->expr->names[oper->id]);
				return 0;
			}
			if( dexpr->roper->args[0]->nargs != oper->nargs ){
				phcalc_eval_newerror(&ctx->err,0,"Incorrect argument number",0);
				return 0;
			}
			if( !phcalc_eval_func(ctx,dexpr,args,res) )
				return 0;
			return 1; }
		assert(0);
		return 0; }
	case PHC_OPER_NEG: {
		if(oper->nargs!=1){
			phcalc_eval_newerror(&ctx->err,0,"Incorrect argument number",0);
			return 0;
		}
		assert( args[0].type==PHC_OBJ_NUM );
		res->type	 = PHC_OBJ_NUM;
		res->ref.num = phcalc_neg(args[0].ref.num);
		return 1; }
	}
	assert(0);
	return 0;
}

int phcalc_getargtypes(phcalc_evalctx *ctx, phcalc_toper *oper, phcalc_typedef *types) {
	int i;
	if(oper->type==PHC_OPER_FNC){
		const char *name = ctx->expr->names[oper->id];
		if( strcmp(name,"Length")==0
			|| strcmp(name,"Sum")==0
			|| strcmp(name,"Norm")==0
			|| strcmp(name,"Average")==0
			|| strcmp(name,"Mean")==0
			|| strcmp(name,"Norm")==0 ){
			if(oper->nargs!=1)
				return 0;
			types[0].type = PHC_OBJ_VECT;
			types[0].vect_depth = 1;
			return 1;
		}
		if( strcmp(name,"Dot")==0 ){
			if(oper->nargs!=2)
				return 0;
			types[0].type = PHC_OBJ_VECT;
			types[0].vect_depth = 1;
			types[1].type = PHC_OBJ_VECT;
			types[1].vect_depth = 1;
			return 1;
		}
		if( phcalc_getdef(ctx->inst,name) != 0 ){
			for(i=0; i<oper->nargs; i++)
				types[i].type = PHC_OBJ_ANY;
			return 1;
		}
	}
	for(i=0; i<oper->nargs; i++)
		types[i].type = PHC_OBJ_NUM;
	return 1;
}

int phcalc_comparetypes_map(phcalc_evalctx *ctx, phcalc_obj *arg, phcalc_typedef *type) {
	if( arg->type==PHC_OBJ_NUM && type->type==PHC_OBJ_NUM )
		return 0;
	if( arg->type==PHC_OBJ_VECT && type->type==PHC_OBJ_NUM )
		return 1;
	if( arg->type==PHC_OBJ_VECT && type->type==PHC_OBJ_VECT ) {
		int depth = phcalc_vector_depth(arg->ref.vect);
		if(depth<=-1)
			return -1;
		if( depth == type->vect_depth )
			return 0;
		if( depth > type->vect_depth )
			return 1;
		return -1;
	}
	if( type->type==PHC_OBJ_ANY ){
		return 0;
	}
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

int phcalc_eval_func(phcalc_evalctx *ctx, phcalc_expr expr, phcalc_obj *args, phcalc_obj *res) {
	int i;
	int nargs = expr->roper->args[0]->nargs;
	int ret;
	phcalc_toper **alist = expr->roper->args[0]->args;
	phcalc_inst inst = phcalc_create_inst();
	phcalc_evalctx *nctx = NEW(phcalc_evalctx);
	nctx->prev	= ctx;
	nctx->inst	= inst;
	nctx->expr	= expr;
	nctx->rec_depth = ctx->rec_depth + 1;
	if( nctx->rec_depth > MAX_REC_DEPTH ){
		phcalc_eval_newerror(&ctx->err,0,"Recursion is too deep",0);
		FREE(nctx);
		return 0;
	}
	phcalc_import(inst,ctx->inst);
	for(i=0; i<nargs; i++){
		if( alist[i]->type!=PHC_OPER_VAR || alist[i]->id==-1 ){
			phcalc_eval_newerror(&ctx->err,0,"Function definition is incorrect, argument isn't a viriable",0);
			FREE(nctx);
			phcalc_destroy_inst(inst);
			return 0;
		}
		phcalc_adddef_obj_nocopy( inst, expr->names[alist[i]->id], args[i], 0 );
	}
	ret = phcalc_eval2(nctx,expr,expr->roper->args[1],res);
	if(!ret)
		ctx->err = nctx->err;
	FREE(nctx);
	phcalc_destroy_inst(inst);
	return ret;
}

int phcalc_vector_depth(phcalc_vect vect) {
	int i;
	int d, depth = -1;
	for(i=0; i<vect.len; i++){
		if( vect.data[i].type == PHC_OBJ_NUM )
			d = 1;
		else if( vect.data[i].type == PHC_OBJ_VECT )
			d = phcalc_vector_depth(vect.data[i].ref.vect);
		else
			return -1;
		if(depth!=-1 && d!=depth)
			return -1;
		else
			depth = d;
	}
	return depth;
}
