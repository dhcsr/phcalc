/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 *
 * Main functions source file (phcalc_main.c)
 *
 **************************************/

#include <stdlib.h>
#include <string.h>

#include "phcalc.h"
#include "phcalc_in.h"

void phcalc_expr_release_rec(phcalc_toper *oper);
phcalc_toper *phcalc_copyexpr_rec(phcalc_toper *oper);

phcalc_inst phcalc_create_inst() {
	phcalc_inst inst = NEW(struct _phcalc_inst);
	inst->defs	= 0;
	return inst;
}

void phcalc_destroy_inst(phcalc_inst inst) {
	free(inst);
}

void phcalc_expr_release(phcalc_inst inst, phcalc_expr expr) {
	phcalc_expr_release_rec(expr->roper);
	if(expr->names!=0)
		free(expr->names);
	free(expr);
}

void phcalc_expr_release_rec(phcalc_toper *oper) {
	int i;
	if(oper==0)
		return;
	for(i=0; i<oper->nargs; i++)
		phcalc_expr_release_rec(oper->args[i]);
	if(oper->args!=0 && oper->nargs!=0)
		free(oper->args);
	free(oper);
}

void phcalc_obj_release(phcalc_inst inst, phcalc_obj obj) {
	// TODO:
}

int phcalc_query(phcalc_inst inst, phcalc_expr expr) {
	if(expr->roper->type == PHC_OPER_DEF){
		phcalc_adddef( inst, expr->names[expr->roper->args[0]->id], expr );
		return 1;
	}
	return 0;
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
	expr->names[i  ] = _strdup(name);
	expr->names[i+1] = 0;
	return i;
}

phcalc_expr phcalc_copyexpr(phcalc_expr expr, phcalc_toper *oper) {
	// TODO: copy only used names if oper!=expr->roper
	int i;
	phcalc_expr nexpr = NEW(struct _phcalc_expr);
	if(expr->names!=0){
		for(i=0; expr->names[i]; i++){}
		nexpr->names = NEWS(char*,i+1);
		for(i=0; expr->names[i]; i++)
			nexpr->names[i] = expr->names[i];
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
	noper->args = NEWS(phcalc_toper*,oper->nargs);
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
		return 3;
	case PHC_OPER_MUL:
	case PHC_OPER_DIV:
		return 4;
	}
	return 0;
}

void phcalc_adddef(phcalc_inst inst, char *name, phcalc_expr expr) {
	phcalc_inst_def *def = (phcalc_inst_def*) malloc(sizeof(phcalc_inst_def));
	def->name	= name;
	def->expr	= phcalc_copyexpr(expr,expr->roper);
	//def->type	= 
	def->next	= inst->defs;
	inst->defs	= def;
}

phcalc_expr phcalc_getdef(phcalc_inst inst, const char *name) {
	phcalc_inst_def *iter = inst->defs;
	while(iter!=0){
		if( strcmp(iter->name,name)==0 )
			return iter->expr;
		iter = iter->next;
	}
	return 0;
}