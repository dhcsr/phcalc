#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <stdio.h>
#include <string.h>

#include "phcalc.h"

int main_routine();
void test_1();
void print_help();

int main(int argc, char *argv[]){
	test_1();
	main_routine();
	_CrtDumpMemoryLeaks();
}

int main_routine() {
	phcalc_inst calc = phcalc_create_inst();
	char buf[512];
	for(;;){
		char *arg;
		gets(buf);
		arg = strchr(buf,' ');
		if(arg!=0){
			arg[0] = 0;
			arg ++;
		}
		if( strcmp(buf,"def")==0 && arg!=0 ){
			phcalc_expr e = phcalc_parse(arg);
			if(e==0){
				printf("Unable to parse\n");
			} else {
				//phcalc_strexpr(calc,e,buf,256);
				//printf("%s\n",buf);
				if( !phcalc_define(calc,e) )
					printf("Error\n");
				phcalc_expr_release(e);
			}
		} else if( strcmp(buf,"undef")==0 && arg!=0 ){
			if( !phcalc_undefine(calc,arg) )
					printf("Error\n");
		} else if( strcmp(buf,"eval")==0 && arg!=0 ){
			phcalc_expr e = phcalc_parse(arg);
			phcalc_obj r;
			if(e!=0){
				if( phcalc_eval(calc,e,&r) ){
					phcalc_strobj(&r,buf,512);
					phcalc_release_obj(calc,&r);
					printf("%s\n",buf);
				} else {
					printf("Error\n");
				}
				phcalc_expr_release(e);
			}
		} else if( strcmp(buf,"exit")==0 ){
			break;
		} else if( strcmp(buf,"help")==0 ){
			print_help();
		} else
			printf("Unknown command\nPrint 'help' for help\n");
	}
	phcalc_destroy_inst(calc);
	return 0;
}

void print_help() {
	puts( 
		"List of commands:\n"
		"def   - define variable or function\n"
		"        def x = 5\n"
		"        def f(x) = x*x\n"
		"undef - remove existing definition\n"
		"        undef x\n"
		"eval  - evaluate expression\n"
		"        eval 1+f(x)\n"
		"help  - help\n"
	);
}