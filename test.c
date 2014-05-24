#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "phcalc.h"
#include "phcalc_in.h"

void test_prim_funcs();
void test_prim_eval();

int dequal(double x, double y);
int requal(phcalc_num x, phcalc_num y);
int requal2(phcalc_num x, double y_val, double y_err);
double dabs(double x);

void test_1() {
	test_prim_funcs();
	test_prim_eval();
	/*FILE *fd = fopen("prog.txt","rt");
	phcalc_inst calc = phcalc_parsefile(fd);*/
}

void test_prim_funcs() {
	phcalc_num
		x = {10,0.1},
		y = {15,0.2},
		z;
	assert( dequal(4.00000001,4.00000002) );
	assert( !dequal(4.0001,4.0002) );
	assert( requal2( phcalc_add(x,y), 25, 0.3 ) );
	assert( requal2( phcalc_sub(x,y), -5, 0.3 ) );
	assert( requal2( phcalc_mul(x,y), 10*15, 3.5 ) );
	assert( requal2( phcalc_div(x,y), 10.0/15.0, 0.015555555555555555 ) );
}

int dequal(double x, double y) {
	return dcmp(x,y)==0;
}

int requal(phcalc_num x, phcalc_num y) {
	return dequal(x.value,y.value) && dequal(x.error,y.error);
}

int requal2(phcalc_num x, double y_val, double y_err) {
	return dequal(x.value,y_val) && dequal(x.error,y_err);
}

void test_prim_eval() {
	phcalc_inst calc = phcalc_create_inst();
	phcalc_expr e1 = phcalc_parse("(1+2)*3/(3+1)");
	phcalc_expr e2 = phcalc_parse("{1,2}+{3,4}");
	phcalc_expr e3 = phcalc_parse("f(x)=x*x");
	phcalc_expr e4 = phcalc_parse("f(2)");
	phcalc_obj r1;
	phcalc_eval(calc,e1,&r1);
	phcalc_query(calc,e3);
	phcalc_eval(calc,e4,&r1);
}