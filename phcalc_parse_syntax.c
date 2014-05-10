/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 *
 * Syntaxis analysator source file (phcalc_parse.c)
 *
 **************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "phcalc.h"
#include "phcalc_in.h"
#include "dynarr.h"
#include "phcalc_parse.h"

#define SYNT_STAKSIZE	64

int phcalc_parse_syntax(ttoken *tokens, int tokens_len, tsnode **stree, tparseerr *err) {
	tsyntaxctx ctx;
	int len;
	tsnode *node;
	ctx.tokens		= tokens;
	ctx.tokens_len	= tokens_len;
	node = phcalc_parse_syntax_program(&ctx,0,&len);
	if(node==0){
		*err = ctx.err;
		return 0;
	}
	return 1;
}

tsnode *new_snode(tsnodetype type) {
	return new_snode2(type,0);
}

tsnode *new_snode2(tsnodetype type, int nodes_len) {
	tsnode *node	= NEW(tsnode);
	node->type		= type;
	if( nodes_len>0 )
		node->nodes	= NEWS(tsnode*,nodes_len);
	else
		node->nodes	= 0;
	node->nodes_len	= nodes_len;
	return node;
}

tsnode *new_snode_token(ttoken *token) {
	tsnode *node  = NEW(tsnode);
	node->type    = SNODE_TOKEN;
	node->token   = token;
	return node;
}

tsnode *phcalc_parse_syntax_program(tsyntaxctx *ctx, int pos, int *len) {
	tsnode *node;
	tsnode *node1	= phcalc_parse_syntax_lines(ctx,pos,len);
	if(node1==0) return 0;
	node = new_snode2(SNODE_PROGRAM,1);
	node->nodes[0] = node1;
	return node;
}

tsnode *phcalc_parse_syntax_lines(tsyntaxctx *ctx, int pos, int *len) {
	if(ctx->tokens[pos].type==TOKEN_BRSO){
		return 0;
	} else {
		int len1,len2;
		tsnode *node1,*node2;
		node1 = phcalc_parse_syntax_expr1(ctx,pos,&len1);
		if(node1==0) return 0;
		if(ctx->tokens[pos+len1].type==TOKEN_EOF) {
			tsnode *node	= new_snode2(SNODE_LINES,1);
			node->nodes[0] = node1;
			return node;
		} else if(ctx->tokens[pos+len1].type==TOKEN_ENDLN) {
			tsnode *node;
			node2 = phcalc_parse_syntax_lines(ctx,pos+len1+1,&len2);
			if(node2==0){
				node	= new_snode2(SNODE_LINES,1);
				node->nodes[0] = node1;
			} else {
				node	= new_snode2(SNODE_LINES,node2->nodes_len+1);
				node->nodes[0] = node1;
				memcpy(node->nodes+1,node2->nodes,sizeof(tsnode*)*node2->nodes_len);
			}
			return node;
		}
		phcalc_parse_newerror(&ctx->err,ctx->tokens[pos+len1].line,ctx->tokens[pos+len1].pos,0,"Unexpected token",0);
		return 0;
	}
}

tsnode *phcalc_parse_syntax_expr1(tsyntaxctx *ctx, int pos, int *len) {
	int len1, len2;
	tsnode *node1, *node2;
	node1 = phcalc_parse_syntax_expr2(ctx,pos,&len1);
	if(node1==0) return 0;
	if( ctx->tokens[pos+len1].type==TOKEN_ASSIGN ){
		tsnode *node;
		node2 = phcalc_parse_syntax_expr2(ctx,pos+len1+1,&len2);
		if(node2==0) return 0;
		node = new_snode2(SNODE_EXPR1,2);
		node->nodes[0]	= node1;
		node->nodes[1]	= node2;
		node->token		= &ctx->tokens[pos+len1];
		*len = len1 + 1 + len2;
		return node;
	}
	*len = len1;
	return node1;
}

tsnode *phcalc_parse_syntax_expr2(tsyntaxctx *ctx, int pos, int *len) {
	int len1, len2;
	tsnode *node1, *node2;
	node1 = phcalc_parse_syntax_expr3(ctx,pos,&len1);
	if(node1==0) return 0;
	while( ctx->tokens[pos+len1].type==TOKEN_PLUS || ctx->tokens[pos+len1].type==TOKEN_MINUS ){
		tsnode *node;
		node2 = phcalc_parse_syntax_expr3(ctx,pos+len1+1,&len2);
		if(node2==0) return 0;
		node = new_snode2(SNODE_EXPR2,2);
		node->nodes[0]	= node1;
		node->nodes[1]	= node2;
		node->token		= &ctx->tokens[pos+len1];
		len1 = len1 + 1 + len2;
		node1 = node;
	}
	*len = len1;
	return node1;
}

tsnode *phcalc_parse_syntax_expr3(tsyntaxctx *ctx, int pos, int *len) {
	int len1, len2;
	tsnode *node1, *node2;
	node1 = phcalc_parse_syntax_name(ctx,pos,&len1);
	if(node1==0) return 0;
	while( ctx->tokens[pos+1].type==TOKEN_ASTER || ctx->tokens[pos+1].type==TOKEN_SLASH ){
		tsnode *node;
		node2 = phcalc_parse_syntax_name(ctx,pos+len1+1,&len2);
		if(node2==0) return 0;
		node = new_snode2(SNODE_EXPR3,2);
		node->nodes[0]	= node1;
		node->nodes[1]	= node2;
		node->token		= &ctx->tokens[pos+len1];
		len1 = len1 + 1 + len2;
		node1 = node;
	}
	*len = len1;
	return node1;
}

tsnode *phcalc_parse_syntax_name(tsyntaxctx *ctx, int pos, int *len) {
	if( ctx->tokens[pos].type==TOKEN_NAME || ctx->tokens[pos].type==TOKEN_NUMBER ) {
		*len = 1;
		return new_snode_token(&ctx->tokens[pos]);
	}
	if(ctx->tokens[pos].type==TOKEN_BRPO) {
		int len1;
		tsnode *node = phcalc_parse_syntax_expr1(ctx,pos+1,&len1);
		if(ctx->tokens[pos+1+len1].type!=TOKEN_BRPC)
			return 0;
		*len = len1 + 2;
		return node;
	}
	phcalc_parse_newerror(&ctx->err,ctx->tokens[pos].line,ctx->tokens[pos].pos,0,"Unexpected token",0);
	return 0;
}