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


/*#define OPER_DEF		0x01		// define a variable ( := )
#define OPER_VAR		0x02		// retreive variable ( x )
#define OPER_FNC		0x03		// execute function ( sin(x) )
#define OPER_NUM		0x04		// number ( 1.0'0.1 )
#define OPER_VCT		0x05		// vector ( {1,2,3} )
#define OPER_PRG		0x06		// program listing ( x:=0; y:=1 )

#define OPER_ADD		0x11
#define OPER_SUB		0x12
#define OPER_MUL		0x13
#define OPER_DIV		0x14
#define OPER_POW		0x15

#define OPER_EQV		0x21
#define OPER_LES		0x22
#define OPER_GRT		0x23
#define OPER_LEE		0x24
#define OPER_GRE		0x25

#define TYPE_NUM		0x01
#define TYPE_VECTOR		0x02
#define TYPE_FUNC		0x108*/

typedef enum _phcalc_opertype {
	PHC_OPER_DEF,		// definition ( := )
	PHC_OPER_VAR,		// variable ( x )
	PHC_OPER_FNC,		// function ( sin(x) )
	PHC_OPER_NUM,		// number ( 1.0'0.1 )
	PHC_OPER_VCT,		// vector ( {1,2,3} )
	PHC_OPER_ELM,		// get element form vector ( x[5] )
	PHC_OPER_PRG,		// program listing ( x:=0; y:=1 )

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
	phcalc_expr expr;
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
	//char *name;
	phcalc_num num;
};

int phcalc_expr_allocname(phcalc_expr expr, const char *name);
//const char *phcalc_expr_getname(phcalc_expr expr, int id);
phcalc_expr phcalc_copyexpr(phcalc_expr expr, phcalc_toper *oper);

void phcalc_adddef(phcalc_inst inst, char *name, phcalc_expr expr);
phcalc_expr phcalc_getdef(phcalc_inst inst, const char *name);

int phcalc_gettype(phcalc_inst inst, const char *name);
//phcalc_num phcalc_getnum(phcalc_inst inst, const char *name);
//phcalc_vector phcalc_getvecotr(phcalc_inst inst, const char *name);

int phcalc_getoperpriority(phcalc_opertype opertype);

int phcalc_execoper(phcalc_inst inst, phcalc_expr expr, phcalc_toper *toper, phcalc_obj *res);
//int phcalc_getstddef(

//int phcalc_getoper



// Math functions:

phcalc_num phcalc_add(phcalc_num x, phcalc_num y);
phcalc_num phcalc_sub(phcalc_num x, phcalc_num y);
phcalc_num phcalc_mul(phcalc_num x, phcalc_num y);
phcalc_num phcalc_div(phcalc_num x, phcalc_num y);
phcalc_num phcalc_pow(phcalc_num x, phcalc_num y);
phcalc_num phcalc_abs(phcalc_num x);
phcalc_num phcalc_sqrt(phcalc_num x);