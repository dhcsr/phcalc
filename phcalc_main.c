/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 *
 * Main functions source file (phcalc_main.c)
 *
 **************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "phcalc.h"
#include "phcalc_in.h"

#define HTDEFS_SIZE		128

void phcalc_expr_release_rec(phcalc_toper *oper);
phcalc_toper *phcalc_copyexpr_rec(phcalc_toper *oper);

int defs_cmp(void *ud, const void *key1, const void *key2);
void defs_free(void *ud, void *key, void *val);

phcalc_inst phcalc_create_inst() {
	phcalc_inst inst	= NEW(struct _phcalc_inst);
	if(inst==0) return 0;
	inst->defs			= ht_create(HTDEFS_SIZE,inst,defs_cmp,defs_free);
	if(inst->defs==0) return 0;
	inst->imports		= 0;
	inst->n_imports		= 0;
	return inst;
}

void phcalc_destroy_inst(phcalc_inst inst) {
	ht_destroy(inst->defs);
	FREE(inst->imports);
	FREE(inst);
}

void phcalc_expr_release(phcalc_expr expr) {
	phcalc_expr_release_rec(expr->roper);
	if(expr->names!=0){		// free name table
		int i;
		for(i=0; expr->names[i]!=0; i++)
			FREE(expr->names[i]);
		FREE(expr->names);
	}
	FREE(expr);
}

void phcalc_expr_release_rec(phcalc_toper *oper) {
	int i;
	if(oper==0)
		return;
	for(i=0; i<oper->nargs; i++)
		phcalc_expr_release_rec(oper->args[i]);
	if(oper->args!=0 && oper->nargs!=0)
		FREE(oper->args);
	FREE(oper);
}

phcalc_obj phcalc_clone_obj(phcalc_inst inst, phcalc_obj *src) {
	int i;
	phcalc_obj newobj = *src;
	newobj.not_delrec = 0;
	switch(src->type){
	case PHC_OBJ_NUM:
		newobj.ref.num = src->ref.num;
		break;
	case PHC_OBJ_VECT:
		newobj.ref.vect = src->ref.vect;
		newobj.ref.vect.data = NEWS(phcalc_obj,newobj.ref.vect.len);
		for(i=0; i<newobj.ref.vect.len; i++)
			newobj.ref.vect.data[i] = phcalc_clone_obj(inst,&src->ref.vect.data[i]);
		break;
	case PHC_OBJ_EXPR:
		newobj.ref.expr = phcalc_copyexpr(src->ref.expr,0);
		break;
	}
	return newobj;
}

void phcalc_release_obj(phcalc_inst inst, phcalc_obj *obj) {
	int i;
	if( obj->not_delrec == 1 )
		return;
	switch(obj->type){
	case PHC_OBJ_NUM:
		break;
	case PHC_OBJ_VECT:
		for(i=0; i<obj->ref.vect.len; i++)
			phcalc_release_obj(inst,&obj->ref.vect.data[i]);
		FREE(obj->ref.vect.data);
		break;
	case PHC_OBJ_EXPR:
		phcalc_expr_release(obj->ref.expr);
		break;
	}
}

phcalc_num phcalc_num_new(double val, double err) {
	phcalc_num num;
	num.value = val;
	num.error = err;
	return num;
}

int phcalc_define(phcalc_inst inst, phcalc_expr expr) {
	if(expr->roper->type == PHC_OPER_DEF){
		assert( expr->roper->args[0]->id >= 0 );
		if( phcalc_getdef(inst,expr->names[expr->roper->args[0]->id])!= 0 ){
			fprintf(stderr,"Name exsits, unable to difine\n");
			return 0;
		}
		phcalc_adddef( inst, expr->names[expr->roper->args[0]->id], expr );
		return 1;
	}
	return 0;
}

int phcalc_undefine(phcalc_inst inst, const char *name) {
	/*phcalc_inst_def *iter = inst->defs;
	phcalc_inst_def *prev = 0;
	while(iter!=0){
		if( strcmp(iter->name,name)==0 ){
			phcalc_release_obj(inst,&iter->obj);
			if(prev==0)
				inst->defs = iter->next;
			else
				prev->next = iter->next;
			FREE(iter->name);
			FREE(iter);
			return 1;
		}
		prev = iter;
		iter = iter->next;
	}*/
	if( ht_get(inst->defs,name) != 0 ){
		ht_delete(inst->defs,name);
		return 1;
	}
	return 0;
}

void phcalc_adddef_obj_nocopy(phcalc_inst inst, const char *name, phcalc_obj obj) {
	/*phcalc_inst_def *def = NEW(phcalc_inst_def);
	def->name	= STRDUP(name);
	def->obj	= obj;
	def->next	= inst->defs;
	inst->defs	= def;*/
	phcalc_obj *nobj = NEW(phcalc_obj);
	*nobj = obj;
	nobj->not_delrec = 1;
	ht_put(inst->defs,STRDUP(name),nobj);
}

void phcalc_adddef(phcalc_inst inst, const char *name, phcalc_expr expr) {
	/*phcalc_inst_def *def = NEW(phcalc_inst_def);
	def->name			= STRDUP(name);
	def->obj.type		= PHC_OBJ_EXPR;
	def->obj.ref.expr	= phcalc_copyexpr(expr,expr->roper);
	def->next			= inst->defs;
	inst->defs			= def;*/
	phcalc_obj *nobj = NEW(phcalc_obj);
	nobj->type		= PHC_OBJ_EXPR;
	nobj->ref.expr	= phcalc_copyexpr(expr,expr->roper);
	ht_put(inst->defs,STRDUP(name),nobj);
}

phcalc_obj *phcalc_getdef(phcalc_inst inst, const char *name) {
	/*phcalc_inst_def *iter = inst->defs;
	while(iter!=0){
		if( strcmp(iter->name,name)==0 )
			return &iter->obj;
		iter = iter->next;
	}*/
	int i;
	phcalc_obj *obj = (phcalc_obj*) ht_get(inst->defs,name);
	if(obj!=0)
		return obj;
	for(i=0; i<inst->n_imports; i++){
		obj = phcalc_getdef(inst->imports[i],name);
		if(obj!=0)
			return obj;
	}
	return 0;
}

int phcalc_import(phcalc_inst inst, phcalc_inst imp) {
	inst->imports = (phcalc_inst*) realloc(inst->imports,sizeof(phcalc_inst)*(inst->n_imports+1));
	if(inst->imports == 0)
		return 0;
	inst->imports[inst->n_imports++] = imp;
	return 1;
}

int phcalc_expr_allocname(phcalc_expr expr, const char *name) {
	int i=0;
	if(expr->names!=0){
	for(i=0; expr->names[i]!=0; i++){
		if(strcmp(expr->names[i],name)==0)
			return i;
	}
	}
	expr->names = (char**) realloc(expr->names,sizeof(char*)*(i+2));
	expr->names[i  ] = STRDUP(name);
	expr->names[i+1] = 0;
	return i;
}

phcalc_expr phcalc_copyexpr(phcalc_expr expr, phcalc_toper *oper) {
	int i;
	phcalc_expr nexpr = NEW(struct _phcalc_expr);
	if(oper==0)
		oper = expr->roper;
	if(expr->names!=0){
		for(i=0; expr->names[i]; i++){}
		nexpr->names = NEWS(char*,i+1);
		for(i=0; expr->names[i]; i++)
			nexpr->names[i] = STRDUP(expr->names[i]);
		nexpr->names[i] = 0;
	} else
		nexpr->names = 0;
	nexpr->roper = phcalc_copyexpr_rec(oper);
	return nexpr;
}

phcalc_toper *phcalc_copyexpr_rec(phcalc_toper *oper) {
	int i;
	phcalc_toper *noper = NEW(phcalc_toper);
	*noper = *oper;
	if( oper->nargs > 0 )
		noper->args = NEWS(phcalc_toper*,oper->nargs);
	else
		noper->args = 0;
	for(i=0; i<oper->nargs; i++)
		noper->args[i] = phcalc_copyexpr_rec(oper->args[i]);
	return noper;
}

int phcalc_gettype(phcalc_inst inst, const char *name) {
	return 0;
}

int phcalc_getoperpriority(phcalc_opertype opertype) {
	switch(opertype){
	case PHC_OPER_DEF:
		return 1;
	case PHC_OPER_ADD:
	case PHC_OPER_SUB:
		return 2;
	case PHC_OPER_MUL:
	case PHC_OPER_DIV:
		return 3;
	case PHC_OPER_POW:
		return 4;
	}
	return 0;
}

FILE *logfile = 0;

void *malloc2(size_t size, const char *str, int line) {
	void *mem = malloc(size);
	if(logfile==0)
		logfile = fopen("log.txt","wt");
	fprintf(logfile,"Alloc 0x%p of %3d at %s,%d\n",mem,size,str,line);
	return mem;
}

char *strdup2(const char *str) {
	char *n = NEWS(char,strlen(str)+1);
	strcpy(n,str);
	return n;
}

int defs_cmp(void *ud, const void *key1, const void *key2){
	return strcmp( (char*)key1, (char*) key2 );
}
void defs_free(void *ud, void *key, void *val){
	FREE(key);
	phcalc_release_obj( (phcalc_inst)ud, (phcalc_obj*) val);
	FREE(val);
}