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
#include <assert.h>

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

phcalc_expr phcalc_parse_compile_expr(tsnode *stree, tparseerr *err) {
	phcalc_expr expr = (phcalc_expr) NEW(struct _phcalc_expr);
	expr->names = 0;
	expr->roper = phcalc_parse_compile_oper(0,expr,stree,err);
	if(expr->roper==0)
		return 0;
	return expr;
}

int phcalc_parse_compile_line(phcalc_inst inst, tsnode *line, tparseerr *err) {
	if( line->type == SNODE_OPER ){
		phcalc_expr expr = (phcalc_expr) NEW(struct _phcalc_expr);
		expr->names = 0;
		expr->roper = phcalc_parse_compile_oper(inst,expr,line,err);
		if(expr->roper==0)
			return 0;
		phcalc_query(inst,expr);
		return 1;
	}
	assert(0);
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
	if( node->type == SNODE_OPER ){
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
		case TOKEN_MINUS:
			if( p_new_oper_p2( oper, PHC_OPER_SUB,
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
		case TOKEN_SLASH:
			if( p_new_oper_p2( oper, PHC_OPER_DIV,
				phcalc_parse_compile_oper(inst,expr,node->nodes[0],err),
				phcalc_parse_compile_oper(inst,expr,node->nodes[1],err) ) )
					return oper;
			break;
		case TOKEN_CARET:
			if( p_new_oper_p2( oper, PHC_OPER_POW,
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
			oper->type		= PHC_OPER_NUM;
			oper->id		= -1;
			oper->nargs		= 0;
			oper->num		= node->token->num;
			return oper;
		}
	} else if( node->type == SNODE_VECTOR ){
		tsnode *list = node->nodes[0];
		int i;
		oper->type		= PHC_OPER_VCT;
		oper->id		= -1;
		oper->nargs		= list->nodes_len;
		oper->args		= NEWS(phcalc_toper*,oper->nargs);
		for(i=0; i<oper->nargs; i++){
			oper->args[i] = phcalc_parse_compile_oper(inst,expr,list->nodes[i],err);
			if(oper->args[i]==0){
				FREE(oper->args);
				FREE(oper);
				return 0;
			}
		}
		return oper;
	} else if( node->type == SNODE_FUNC ){
		tsnode *list = node->nodes[0];
		int i;
		oper->type		= PHC_OPER_FNC;
		oper->id		= phcalc_expr_allocname(expr,node->token->str);
		oper->nargs		= list->nodes_len;
		oper->args		= NEWS(phcalc_toper*,oper->nargs);
		for(i=0; i<oper->nargs; i++){
			oper->args[i] = phcalc_parse_compile_oper(inst,expr,list->nodes[i],err);
			if(oper->args[i]==0){
				FREE(oper->args);
				FREE(oper);
				return 0;
			}
		}
		return oper;
	} else if( node->type == SNODE_NEGATE ){
		oper->type		= PHC_OPER_NEG;
		oper->id		= -1;
		oper->nargs		= 1;
		oper->args		= NEWS(phcalc_toper*,1);
		oper->args[0]	= phcalc_parse_compile_oper(inst,expr,node->nodes[0],err);
		if(oper->args[0]==0){
			FREE(oper);
			return 0;
		}
		return oper;
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