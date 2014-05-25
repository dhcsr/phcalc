/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 * https://github.com/dhcsr/phcalc
 *
 * Inner header file (phcalc_in.h)
 * This file used only inside of the library
 * It conatains declaration used only
 * in sources of the library
 *
 **************************************/

#pragma once

#define NEW(type)				((type*) malloc(sizeof(type)))
#define NEWS(type,cnt)			((type*) malloc(sizeof(type)*(cnt)))
#define REALCS(ref,type,cnt)	((type*) realloc(ref,sizeof(type)*(cnt)))
#define FREE(ref)				free(ref);


typedef enum _phcalc_opertype {
	PHC_OPER_DEF,		// definition ( := )
	PHC_OPER_VAR,		// variable ( x )
	PHC_OPER_FNC,		// function ( sin(x) )
	PHC_OPER_NUM,		// number ( 1.0'0.1 )
	PHC_OPER_VCT,		// vector ( {1,2,3} )
	//PHC_OPER_ELM,		// get element form vector ( x[5] )
	//PHC_OPER_PRG,		// program listing ( x:=0; y:=1 )

	PHC_OPER_NEG,		// negate number ( - )
	PHC_OPER_ADD,		// addition ( + )
	PHC_OPER_SUB,		// substraction ( - )
	PHC_OPER_MUL,		// multiplication ( * )
	PHC_OPER_DIV,		// division ( / )
	PHC_OPER_POW		// power ( ^ )
} phcalc_opertype;

//typedef enum _phcalc_deftype {
//};

/*
Operators priorities

1	:=
2	= < >
3	+ -
4	* /
5	^

*/

#define PHC_OPERF_PEXPR		0x0001		// pass argument as expression, used in function definitions

typedef struct _phcalc_toper phcalc_toper;
typedef struct _phcalc_vector phcalc_vector;
typedef struct _phcalc_inst_def phcalc_inst_def;

struct _phcalc_inst_def {
	int type;
	char *name;
	//phcalc_expr expr;
	phcalc_obj obj;
	phcalc_inst_def *next;
};

struct _phcalc_inst {
	int cd;
	phcalc_inst_def *defs;
};

struct _phcalc_expr {
	char **names;
	phcalc_toper *roper;
};

struct _phcalc_toper {
	phcalc_opertype type;
	int id;
	int nargs;
	int flags;
	phcalc_toper **args;
	phcalc_num num;
};

typedef struct _phcalc_typedef {
	phcalc_type_basic type;
	int vect_depth;
} phcalc_typedef;

typedef struct _tparseerr {
	int line, pos;
	int code;
	const char *desc;
	char buf[32];
} tparseerr;

typedef struct _tevalerr {
	int code;
	const char *desc;
	char buf[64];
} tevalerr;

typedef struct _phcalc_evalctx phcalc_evalctx;
struct _phcalc_evalctx {
	phcalc_evalctx *prev;
	phcalc_inst inst;
	phcalc_expr expr;
	tevalerr err;
	int rec_depth;
	void *stack;
};

int phcalc_expr_allocname(phcalc_expr expr, const char *name);
//const char *phcalc_expr_getname(phcalc_expr expr, int id);
phcalc_expr phcalc_copyexpr(phcalc_expr expr, phcalc_toper *oper);

void phcalc_adddef(phcalc_inst inst, char *name, phcalc_expr expr);
void phcalc_adddef_obj_nocopy(phcalc_inst inst, char *name, phcalc_obj obj);
phcalc_obj *phcalc_getdef(phcalc_inst inst, const char *name);

int phcalc_gettype(phcalc_inst inst, const char *name);
//phcalc_num phcalc_getnum(phcalc_inst inst, const char *name);
//phcalc_vector phcalc_getvecotr(phcalc_inst inst, const char *name);

int phcalc_getoperpriority(phcalc_opertype opertype);


// Error handling functions:

void phcalc_eval_newerror(tevalerr *err, int code, const char *desc, const char *buf);
void phcalc_eval_printerror(FILE *fd, tevalerr *err);

void phcalc_parse_newerror(tparseerr *err, int line, int pos, int code, const char *desc, const char *buf);
void phcalc_parse_printerror(FILE *fd, tparseerr *err);


// Math functions:

int dcmp(double x, double y);
double dabs(double x);
int phcalc_cmp(phcalc_num x, phcalc_num y);

int phcalc_const(const char *name, phcalc_num *res);
int phcalc_mfunc(phcalc_evalctx *ctx, const char *name, phcalc_obj *args, int nargs, phcalc_obj *res);

phcalc_num phcalc_neg(phcalc_num x);
phcalc_num phcalc_add(phcalc_num x, phcalc_num y);
phcalc_num phcalc_sub(phcalc_num x, phcalc_num y);
phcalc_num phcalc_mul(phcalc_num x, phcalc_num y);
phcalc_num phcalc_div(phcalc_num x, phcalc_num y);
phcalc_num phcalc_pow(phcalc_num x, phcalc_num y);

phcalc_num phcalc_abs(phcalc_num x);		// Abs
phcalc_num phcalc_sqrt(phcalc_num x);		// Sqrt
int phcalc_average(phcalc_num *res, phcalc_vect vector);
int phcalc_mean(phcalc_num *res, phcalc_vect vector);