#include <stdlib.h>
#include <string.h>

#include "phcalc.h"
#include "phcalc_in.h"

void phcalc_adddef(char *name, phcalc_expr expr);

void phcalc_expr_release_rec(phcalc_toper *oper);

phcalc_inst phcalc_create_inst() {
	phcalc_inst inst = (phcalc_inst) malloc(sizeof(struct _phcalc_inst));

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

int phcalc_query(phcalc_inst inst, phcalc_expr expr) {
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
	return 0;
}

int phcalc_execoper(phcalc_inst inst, phcalc_expr expr, phcalc_toper toper) {
	switch(toper.type){
	case PHC_OPER_VAR: {
		int type = phcalc_gettype(inst,expr->names[toper.id]);

		break; }
	}
	return 1;
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

void phcalc_adddef(char *name, phcalc_expr expr) {

}