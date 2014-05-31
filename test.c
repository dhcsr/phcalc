/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 * https://github.com/dhcsr/phcalc
 *
 * Testing source file
 *
 **************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "phcalc.h"
#include "phcalc_in.h"

void test_prim_funcs();
void test_prim_eval();
void test_def();
void test_csv();

int dequal(double x, double y);
int requal(phcalc_num x, phcalc_num y);
int requal2(phcalc_num x, double y_val, double y_err);
double dabs(double x);

void test_1() {
	test_prim_funcs();
	test_prim_eval();
	test_csv();
	/*{
		FILE *fd = fopen("prog.txt","rt");
		phcalc_inst calc = phcalc_parsefile(fd);
		phcalc_destroy_inst(calc);
	}*/
}

void test_prim_funcs() {
	phcalc_num
		x = {10,0.1},
		y = {15,0.2};
	assert( dequal(4.00000001,4.00000002) );
	assert( !dequal(4.0001,4.0002) );
	assert( requal2( phcalc_add(x,y), 25, 0.3 ) );
	assert( requal2( phcalc_sub(x,y), -5, 0.3 ) );
	assert( requal2( phcalc_mul(x,y), 10*15, 3.52 ) );
	assert( requal2( phcalc_div(x,y), 10.0/15.0, 0.015765765765765716 ) );
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
	phcalc_obj r1;
	phcalc_eval(calc,e1,&r1);
	assert(r1.type==PHC_OBJ_NUM);
	assert( requal2( r1.ref.num, 9.0/4.0, 0.0 ) );
	phcalc_expr_release(e2);
	phcalc_expr_release(e1);
	phcalc_destroy_inst(calc);
}

void test_def() {
	phcalc_inst calc = phcalc_create_inst();
	phcalc_expr e1 = phcalc_parse("x=100+3");
	phcalc_expr e2 = phcalc_parse("2*x");
	phcalc_obj r1;
	assert( phcalc_define(calc,e1) );
	assert( phcalc_eval(calc,e2,&r1) );
	assert( r1.type==PHC_OBJ_NUM );
	assert( requal2( r1.ref.num, (100+3)*2, 0.0 ) );
	phcalc_release_obj(calc,&r1);
	phcalc_expr_release(e1);
	phcalc_expr_release(e2);
	phcalc_destroy_inst(calc);
}

void test_csv() {
	const char *fields[] = { "n", "x" };
	phcalc_inst calc = phcalc_create_inst();
	FILE *file = fopen("test.csv","rt");
	if(file==0){
		printf("Unable to test CSV-loader: test.csv not found\n");
		return;
	}
	assert( phcalc_csv_load(calc,file,fields,2,0) );
	phcalc_destroy_inst(calc);
}