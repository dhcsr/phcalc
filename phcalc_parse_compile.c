/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 * https://github.com/dhcsr/phcalc
 *
 * Compiler for parser (phcalc_parse_error.c)
 *
 **************************************/

#include <stdlib.h>
#include <string.h>

#include "phcalc.h"
#include "phcalc_in.h"
#include "phcalc_parse.h"

int p_new_oper_p2(phcalc_toper *oper, phcalc_opertype type, phcalc_toper *arg1, phcalc_toper *arg2);

phcalc_inst phcalc_parse_compile_inst(tsnode *stree, tparseerr *err) {
	//tsnode *lines;
	phcalc_inst inst = phcalc_create_inst();
	if(stree->type!=SNODE_PROGRAM || stree->nodes_len < 1){
		return 0;
	}
	if( !phcalc_parse_compile_lines(inst,stree->nodes[0],err) ){
		return 0;
	}
	return inst;
}

int phcalc_parse_compile_line(phcalc_inst inst, tsnode *line, tparseerr *err) {
	if( line->type >= SNODE_EXPR1 && line->type <= SNODE_EXPR3 ){
		phcalc_expr expr = (phcalc_expr) NEW(struct _phcalc_expr);
		expr->names = 0;
		expr->roper = phcalc_parse_compile_oper(inst,expr,line,err);
		if(expr->roper==0)
			return 0;
		return 1;
	}
	return 0;
}

int phcalc_parse_compile_lines(phcalc_inst inst, tsnode *lines, tparseerr *err) {
	int i;
	if(lines->type!=SNODE_LINES){
		return 0;
	}
	for(i=0; i<lines->nodes_len; i++){
		if( !phcalc_parse_compile_line(inst,lines->nodes[i],err) )
			return 0;
	}
	return 1;
}

phcalc_toper *phcalc_parse_compile_oper(phcalc_inst inst, phcalc_expr expr, tsnode *node, tparseerr *err) {
	phcalc_toper *oper = NEW(phcalc_toper);
	if( node->type >= SNODE_EXPR1 && node->type <= SNODE_EXPR3 ){
		switch(node->token->type){
		case TOKEN_ASSIGN:
			if( p_new_oper_p2( oper, PHC_OPER_DEF,
				phcalc_parse_compile_oper(inst,expr,node->nodes[0],err),
				phcalc_parse_compile_oper(inst,expr,node->nodes[1],err) ) )
					return oper;
			break;
		case TOKEN_PLUS:
			if( p_new_oper_p2( oper, PHC_OPER_ADD,
				phcalc_parse_compile_oper(inst,expr,node->nodes[0],err),
				phcalc_parse_compile_oper(inst,expr,node->nodes[1],err) ) )
					return oper;
			break;
		case TOKEN_ASTER:
			if( p_new_oper_p2( oper, PHC_OPER_MUL,
				phcalc_parse_compile_oper(inst,expr,node->nodes[0],err),
				phcalc_parse_compile_oper(inst,expr,node->nodes[1],err) ) )
					return oper;
			break;
		//default:
		}
	} else if( node->type == SNODE_TOKEN ){
		switch(node->token->type){
		case TOKEN_NAME:
			oper->type		= PHC_OPER_VAR;
			oper->id		= phcalc_expr_allocname(expr,node->token->str);
			oper->nargs		= 0;
			return oper;
		case TOKEN_NUMBER:
			// TODO:
			oper->type		= PHC_OPER_NUM;
			oper->id		= -1;
			oper->nargs		= 0;
			oper->num		= node->token->num;
			return oper;
		}
	}
	phcalc_parse_newerror(err,node->src_line,-1,0,"Unknown operation",0);
	FREE(oper);
	return 0;
}

int p_new_oper_p2(phcalc_toper *oper, phcalc_opertype type, phcalc_toper *arg1, phcalc_toper *arg2) {
	if( arg1==0 || arg2==0 )
		return 0;
	oper->type		= type;
	oper->id		= -1;
	oper->nargs		= 2;
	oper->args		= (phcalc_toper**) malloc( sizeof(phcalc_toper*)*oper->nargs );
	oper->args[0]	= arg1;
	oper->args[1]	= arg2;
	return 1;
}