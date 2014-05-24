/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 * https://github.com/dhcsr/phcalc
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
	*stree = node;
	return 1;
}

int phcalc_parse_syntax_expr(ttoken *tokens, int tokens_len, tsnode **stree, tparseerr *err) {
	tsyntaxctx ctx;
	int len;
	tsnode *node;
	ctx.tokens		= tokens;
	ctx.tokens_len	= tokens_len;
	node = phcalc_parse_syntax_expr1(&ctx,0,&len);
	if(node==0){
		*err = ctx.err;
		return 0;
	}
	if(tokens[len].type!=TOKEN_EOF){
		phcalc_parse_newerror(err,tokens[len].line,tokens[len].pos,0,"Unexpected token, EOF expected",0);
		return 0;
	}
	*stree = node;
	return 1;
}

tsnode *new_snode(tsnodetype type, int src_line) {
	return new_snode2(type,0,src_line);
}

tsnode *new_snode2(tsnodetype type, int nodes_len, int src_line) {
	tsnode *node	= NEW(tsnode);
	node->type		= type;
	if( nodes_len>0 )
		node->nodes	= NEWS(tsnode*,nodes_len);
	else
		node->nodes	= 0;
	node->nodes_len	= nodes_len;
	node->src_line	= src_line;
	return node;
}

tsnode *new_snode_token(ttoken *token) {
	tsnode *node	= NEW(tsnode);
	node->type		= SNODE_TOKEN;
	node->token		= token;
	node->nodes_len	= 0;
	node->src_line	= token->line;
	return node;
}

tsnode *phcalc_parse_syntax_program(tsyntaxctx *ctx, int pos, int *len) {
	tsnode *node;
	tsnode *node1	= phcalc_parse_syntax_lines(ctx,pos,len);
	if(node1==0) return 0;
	node = new_snode2(SNODE_PROGRAM,1,ctx->tokens[pos].line);
	node->nodes[0] = node1;
	return node;
}

tsnode *phcalc_parse_syntax_lines(tsyntaxctx *ctx, int pos, int *len) {
	int offset = pos;
	tsnode **lines;
	int nlines = 0;
	tsnode *node;
	dynarr_create( (void**)&lines, sizeof(tsnode*) );
	while( ctx->tokens[offset].type != TOKEN_EOF ){
		if(ctx->tokens[offset].type==TOKEN_BRSO){
			phcalc_parse_newerror(&ctx->err,ctx->tokens[offset].line,ctx->tokens[offset].pos,0,
				"Unexpected token",gettokenname(ctx->tokens[offset].type));
			dynarr_desable( (void**)lines );
			FREE(lines);
			return 0;
		} else {
			int len1;
			tsnode *node1;
			node1 = phcalc_parse_syntax_expr1(ctx,offset,&len1);
			if(node1==0){
				dynarr_desable( (void**)&lines );
				FREE(lines);
				return 0;
			}
			dynarr_add( (void**)&lines, node1 );
			offset += len1;
			if( ctx->tokens[offset].type != TOKEN_ENDLN ){
				phcalc_parse_newerror(&ctx->err,ctx->tokens[offset].line,ctx->tokens[offset].pos,0,
					"Unexpected token",gettokenname(ctx->tokens[offset].type));
				dynarr_desable( (void**)&lines );
				FREE(lines);
				return 0;
			}
			offset ++;
		}
	}
	nlines = dynarr_desable( (void**)&lines );
	node			= new_snode(SNODE_LINES,ctx->tokens[pos].line);
	node->nodes		= lines;
	node->nodes_len	= nlines;
	return node;
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
		node = new_snode2(SNODE_OPER,2,ctx->tokens[pos].line);
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
		node = new_snode2(SNODE_OPER,2,ctx->tokens[pos].line);
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
	node1 = phcalc_parse_syntax_expr4(ctx,pos,&len1);
	if(node1==0) return 0;
	while( ctx->tokens[pos+len1].type==TOKEN_ASTER || ctx->tokens[pos+len1].type==TOKEN_SLASH ){
		tsnode *node;
		node2 = phcalc_parse_syntax_expr4(ctx,pos+len1+1,&len2);
		if(node2==0) return 0;
		node = new_snode2(SNODE_OPER,2,ctx->tokens[pos].line);
		node->nodes[0]	= node1;
		node->nodes[1]	= node2;
		node->token		= &ctx->tokens[pos+len1];
		len1 = len1 + 1 + len2;
		node1 = node;
	}
	*len = len1;
	return node1;
}

tsnode *phcalc_parse_syntax_expr4(tsyntaxctx *ctx, int pos, int *len) {
	int len1, len2;
	tsnode *node1, *node2;
	node1 = phcalc_parse_syntax_name(ctx,pos,&len1);
	if(node1==0) return 0;
	while( ctx->tokens[pos+len1].type==TOKEN_CARET ){
		tsnode *node;
		node2 = phcalc_parse_syntax_name(ctx,pos+len1+1,&len2);
		if(node2==0) return 0;
		node = new_snode2(SNODE_OPER,2,ctx->tokens[pos].line);
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
	if( ctx->tokens[pos].type==TOKEN_NUMBER ) {
		*len = 1;
		return new_snode_token(&ctx->tokens[pos]);
	}
	if( ctx->tokens[pos].type==TOKEN_NAME ) {
		if( ctx->tokens[pos+1].type==TOKEN_BRPO ){
			int len1;
			tsnode *node1;
			tsnode *node;
			node1 = phcalc_parse_syntax_list(ctx,pos+2,&len1);
			if( ctx->tokens[pos+2+len1].type != TOKEN_BRPC){
				phcalc_parse_newerror(&ctx->err,ctx->tokens[pos+2+len1].line,ctx->tokens[pos+2+len1].pos,0,
					"Unexpected token",gettokenname(ctx->tokens[pos+2+len1].type));
				return 0;
			}
			node = new_snode2(SNODE_FUNC,1,ctx->tokens[pos].line);
			node->token = &ctx->tokens[pos];
			node->nodes[0] = node1;
			*len = len1 + 3;
			return node;
		}
		*len = 1;
		return new_snode_token(&ctx->tokens[pos]);
	}
	if(ctx->tokens[pos].type==TOKEN_BRPO) {
		int len1;
		tsnode *node = phcalc_parse_syntax_expr1(ctx,pos+1,&len1);
		if( ctx->tokens[pos+1+len1].type != TOKEN_BRPC){
			phcalc_parse_newerror(&ctx->err,ctx->tokens[pos+1+len1].line,ctx->tokens[pos+1+len1].pos,0,
				"Unexpected token",gettokenname(ctx->tokens[pos+1+len1].type));
			return 0;
		}
		*len = len1 + 2;
		return node;
	}
	if(ctx->tokens[pos].type==TOKEN_BRCO) {
		int len1;
		tsnode *node1;
		tsnode *node;
		node1 = phcalc_parse_syntax_list(ctx,pos+1,&len1);
		if( ctx->tokens[pos+1+len1].type != TOKEN_BRCC){
			phcalc_parse_newerror(&ctx->err,ctx->tokens[pos+1+len1].line,ctx->tokens[pos+1+len1].pos,0,
				"Unexpected token",gettokenname(ctx->tokens[pos+1+len1].type));
			return 0;
		}
		node = new_snode2(SNODE_VECTOR,1,ctx->tokens[pos].line);
		node->nodes[0] = node1;
		*len = len1 + 2;
		return node;
	}
	phcalc_parse_newerror(&ctx->err,ctx->tokens[pos].line,ctx->tokens[pos].pos,0,
		"Unexpected token",gettokenname(ctx->tokens[pos].type));
	return 0;
}

tsnode *phcalc_parse_syntax_list(tsyntaxctx *ctx, int pos, int *len) {
	int offset = pos;
	tsnode **list;
	int nlist = 0;
	tsnode *node;
	dynarr_create( (void**)&list, sizeof(tsnode*) );
	for(;;){
		int len1;
		tsnode *node1;
		node1 = phcalc_parse_syntax_expr1(ctx,offset,&len1);
		if(node1==0){
			dynarr_desable( (void**)&list );
			FREE(list);
			return 0;
		}
		dynarr_add( (void**)&list, node1 );
		offset += len1;
		if( ctx->tokens[offset].type != TOKEN_COMMA )
			break;
		offset ++;
	}
	nlist = dynarr_desable( (void**)&list );
	node			= new_snode(SNODE_LIST,ctx->tokens[pos].line);
	node->nodes		= list;
	node->nodes_len	= nlist;
	*len = offset - pos;
	return node;
}