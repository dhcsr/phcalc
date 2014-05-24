/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 * https://github.com/dhcsr/phcalc
 *
 * Main header file (phcalc.h)
 * This file contains declaration of library functions
 * You need to include it in your project
 * if you're going to use it
 **************************************/

#pragma once

#include <stdio.h>

typedef struct _phcalc_inst *phcalc_inst;
typedef struct _phcalc_expr *phcalc_expr;
typedef struct _phcalc_num phcalc_num;
typedef struct _phcalc_obj phcalc_obj;
typedef struct _phcalc_vect phcalc_vect;

typedef enum _phcalc_type_basic {
	PHC_OBJ_NUM,
	PHC_OBJ_VECT,
	PHC_OBJ_EXPR
} phcalc_type_basic;

struct _phcalc_vect {
	int len;
	phcalc_obj *data;
};

struct _phcalc_num {
	double value;
	double error;
};

struct _phcalc_obj {
	phcalc_type_basic type;
	union {
		phcalc_num num;
		phcalc_vect vect;
		//phcalc_expr expr;
	} ref;
};

// Create new instance
phcalc_inst phcalc_create_inst();

// Destroy instance and release memory
void phcalc_destroy_inst(phcalc_inst inst);

// Import definition from `src` into `inst`
void phcalc_import(phcalc_inst inst, phcalc_inst src);

// Parse expression
phcalc_expr phcalc_parse(const char *str);

// Parse file into new instance
phcalc_inst phcalc_parsefile(FILE *fd);


int phcalc_strexpr(phcalc_inst inst, phcalc_expr expr, char *str, int len);
int phcalc_strobj(phcalc_obj *obj, char *str, int len);
void phcalc_expr_release(phcalc_inst inst, phcalc_expr expr);

phcalc_obj phcalc_clone_obj(phcalc_inst inst, phcalc_obj *src);
void phcalc_release_obj(phcalc_inst inst, phcalc_obj *obj);
void phcalc_release_objs(phcalc_inst inst, phcalc_obj *obj, int count);

/*
Program sample:
x := 100'5
y := 200~0.05
len(x,y) := x*x + y*y
l := len(x,y)
*/

int phcalc_query(phcalc_inst inst, phcalc_expr expr);
int phcalc_eval(phcalc_inst inst, phcalc_expr expr, phcalc_obj *res);

//int phcalc_getoperpriority(int opertype);