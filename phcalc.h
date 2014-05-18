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

struct _phcalc_obj {
	enum { PHC_OBJ_NUM, PHC_OBJ_VECT } type;
	union {
		phcalc_num *num;
		phcalc_vect *vect;
	} ref;
};

struct _phcalc_vect {
	int len;
	phcalc_obj *vect;
};

struct _phcalc_num {
	double value;
	double error;
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
void phcalc_obj_release(phcalc_inst inst, phcalc_obj obj);

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