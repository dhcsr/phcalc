/*

*/
#pragma once

typedef struct _phcalc_inst *phcalc_inst;
typedef struct _phcalc_expr *phcalc_expr;
typedef struct _phcalc_num phcalc_num;
typedef struct _phcalc_obj phcalc_obj;
typedef struct _phcalc_vect phcalc_vect;

struct _phcalc_obj {
	enum { PHC_OBJ_NUM, PHC_OBJ_LIST } type;
	void *ref;
};

struct _phcalc_vect {
	int len;
	phcalc_obj *list;
};

struct _phcalc_num {
	double value;
	double error;
};

phcalc_inst phcalc_create_inst();
void phcalc_destroy_inst(phcalc_inst inst);

phcalc_expr phcalc_parse(phcalc_inst inst, const char *str);
int phcalc_strexpr(phcalc_inst inst, phcalc_expr expr, char *str, int len);
void phcalc_expr_release(phcalc_inst inst, phcalc_expr expr);

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