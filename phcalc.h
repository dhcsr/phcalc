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

/*
	Types of object
	Used in `phcalc_obj::type`
*/
typedef enum _phcalc_type_basic {
	PHC_OBJ_NUM,	// number
	PHC_OBJ_VECT,	// vector
	PHC_OBJ_EXPR,	// expression
	PHC_OBJ_ANY		// any, used only to match any type
} phcalc_type_basic;

/*
	Vector
	Used in `phcalc_obj::ref.vect`
*/
struct _phcalc_vect {
	int len;
	phcalc_obj *data;
};

/*
	Number with error (tolerance)
*/
struct _phcalc_num {
	double value;
	double error;
};

struct _phcalc_obj {
	phcalc_type_basic type;
	union {
		phcalc_num num;
		phcalc_vect vect;
		phcalc_expr expr;
	} ref;
	char not_delrec;		// do not delete inner data, i.e. it's reference to another object
};

// Create new instance
phcalc_inst phcalc_create_inst();

// Destroy instance and release memory
void phcalc_destroy_inst(phcalc_inst inst);

// Import definition from `src` into `inst`
int phcalc_import(phcalc_inst inst, phcalc_inst imp);

// Release imports
int phcalc_release_imports(phcalc_inst inst);

// Parse expression
phcalc_expr phcalc_parse(const char *str);

// Parse file into new instance
phcalc_inst phcalc_parsefile(FILE *fd);

// Convert expression to string
int phcalc_strexpr(phcalc_inst inst, phcalc_expr expr, char *str, int len);

// Release (destroy) expression and free memory
void phcalc_expr_release(phcalc_expr expr);

// Convect object to string
int phcalc_strobj(phcalc_obj *obj, char *str, int len);

// Clone (copy) object, inner objects are cloned as well
phcalc_obj phcalc_clone_obj(phcalc_inst inst, phcalc_obj *src);

// Release object, data at `obj` is not freed! (i.e. you can use (..., &obj), otherwise call `free` subsequently)
void phcalc_release_obj(phcalc_inst inst, phcalc_obj *obj);

// Release object stored in an array, see `phcalc_release_obj` for details
void phcalc_release_objs(phcalc_inst inst, phcalc_obj *obj, int count);

// Create new number (val,err)
phcalc_num phcalc_num_new(double val, double err);

// Define variable or function, `expr` is copied
int phcalc_define(phcalc_inst inst, phcalc_expr expr);

// Undefine variable or function
int phcalc_undefine(phcalc_inst inst, const char *name);

// Get definition (do not edit return!)
phcalc_obj *phcalc_getdef(phcalc_inst inst, const char *name);

// Evaluate (calculate) expression, result is stored to `res`
int phcalc_eval(phcalc_inst inst, phcalc_expr expr, phcalc_obj *res);

// Load csv file
int phcalc_csv_load(phcalc_inst inst, FILE *file, const char **fields, int n_fields, unsigned long flags);

// Save csv file
//int phcalc_csv_save(phcalc_inst inst, FILE *file, const char **fields, int n_fields, unsigned long flags);