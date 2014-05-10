#pragma once

typedef enum _ttokentype {
	TOKEN_NAME,
	TOKEN_NUMBER,
	TOKEN_ASSIGN,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_ASTER,
	TOKEN_SLASH,
	TOKEN_ENDLN,	// ;
	TOKEN_BRPO,		// (
	TOKEN_BRPC,		// )
	TOKEN_BRCO,		// {
	TOKEN_BRCC,		// }
	TOKEN_BRSO,		// {
	TOKEN_BRSC,		// }
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
	SNODE_EXPR3
} tsnodetype;

typedef struct _ttoken {
	ttokentype type;
	char *str;
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
} tsnode;

typedef struct _tsyntaxctx {
	ttoken *tokens;
	int tokens_len;
} tsyntaxctx;

int phcalc_parse_lexic(FILE *fd, ttoken **data, int *len, char *err);
ttoken new_token(ttokentype type, const char *name);

int phcalc_parse_syntax(ttoken *tokens, int tokens_len, tsnode **stree);
tsnode *new_snode(tsnodetype type);
tsnode *new_snode2(tsnodetype type, int nodes_len);
tsnode *new_snode_token(ttoken *token);
tsnode *phcalc_parse_syntax_program(tsyntaxctx *ctx, int pos, int *len);
tsnode *phcalc_parse_syntax_lines(tsyntaxctx *ctx, int pos, int *len);
tsnode *phcalc_parse_syntax_expr1(tsyntaxctx *ctx, int pos, int *len);		// =
tsnode *phcalc_parse_syntax_expr2(tsyntaxctx *ctx, int pos, int *len);		// + -
tsnode *phcalc_parse_syntax_expr3(tsyntaxctx *ctx, int pos, int *len);		// * /
tsnode *phcalc_parse_syntax_name(tsyntaxctx *ctx, int pos, int *len);