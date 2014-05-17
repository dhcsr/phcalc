/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 * https://github.com/dhcsr/phcalc
 *
 * phcalc_parse.h
 * Inner include file of parser
 *
 **************************************/

#pragma once

typedef enum _ttokentype {
	TOKEN_NAME,
	TOKEN_NUMBER,
	TOKEN_ASSIGN,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_ASTER,
	TOKEN_SLASH,
	TOKEN_COMMA,	// ,
	TOKEN_ENDLN,	// ;
	TOKEN_BRPO,		// (
	TOKEN_BRPC,		// )
	TOKEN_BRCO,		// {
	TOKEN_BRCC,		// }
	TOKEN_BRSO,		// [
	TOKEN_BRSC,		// ]
	TOKEN_EOF,

	TOKEN_NEW,
	TOKEN_SPACE,
	//TOKEN_OPER
} ttokentype;

typedef enum _tsnodetype {
	/*SNODE_LINEBEGIN,
	SNODE_TOKEN,
	SNODE_DECL,
	SNODE_EXPR,
	SNODE_LINE,
	SNODE_BLOCK*/
	SNODE_TOKEN,
	SNODE_PROGRAM,
	SNODE_LINES,
	SNODE_EXPR1,
	SNODE_EXPR2,
	SNODE_EXPR3,
	SNODE_LIST,
	SNODE_FUNC
} tsnodetype;

typedef struct _tparseerr {
	int line, pos;
	int code;
	const char *desc;
	char buf[32];
} tparseerr;

typedef struct _ttoken {
	ttokentype type;
	char *str;
	int line, pos;
	phcalc_num num;
} ttoken;

//typedef struct _tsnodeexpr {
//	phcalc_opertype oper;
//	int nargs;
//	ttoken *token;
//} tsnodeexpr;

typedef struct _tsnode {
	tsnodetype type;
	ttoken *token, *token1;
	//tsnodeexpr *expr;
	struct _tsnode **nodes;
	int nodes_len;
	int src_line;
} tsnode;

typedef struct _tsyntaxctx {
	ttoken *tokens;
	int tokens_len;
	tparseerr err;
} tsyntaxctx;

void phcalc_parse_newerror(tparseerr *err, int line, int pos, int code, const char *desc, const char *buf);
void phcalc_parse_printerror(FILE *fd, tparseerr *err);

int phcalc_parse_lexic(FILE *fd, ttoken **data, int *len, tparseerr *err);
int phcalc_parse_number(const char *str, phcalc_num *num);
ttoken new_token(ttokentype type, const char *name, int line, int pos);
const char *gettokenname(ttokentype token);

int phcalc_parse_syntax(ttoken *tokens, int tokens_len, tsnode **stree, tparseerr *err);
tsnode *new_snode(tsnodetype type, int src_line);
tsnode *new_snode2(tsnodetype type, int nodes_len, int src_line);
tsnode *new_snode_token(ttoken *token);
tsnode *phcalc_parse_syntax_program(tsyntaxctx *ctx, int pos, int *len);
tsnode *phcalc_parse_syntax_lines(tsyntaxctx *ctx, int pos, int *len);
tsnode *phcalc_parse_syntax_expr1(tsyntaxctx *ctx, int pos, int *len);		// =
tsnode *phcalc_parse_syntax_expr2(tsyntaxctx *ctx, int pos, int *len);		// + -
tsnode *phcalc_parse_syntax_expr3(tsyntaxctx *ctx, int pos, int *len);		// * /
tsnode *phcalc_parse_syntax_name(tsyntaxctx *ctx, int pos, int *len);
tsnode *phcalc_parse_syntax_list(tsyntaxctx *ctx, int pos, int *len);

phcalc_inst phcalc_parse_compile_inst(tsnode *stree, tparseerr *err);
int phcalc_parse_compile_line(phcalc_inst inst, tsnode *line, tparseerr *err);
int phcalc_parse_compile_lines(phcalc_inst inst, tsnode *lines, tparseerr *err);
phcalc_toper *phcalc_parse_compile_oper(phcalc_inst inst, phcalc_expr expr, tsnode *node, tparseerr *err);