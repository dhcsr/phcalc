#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <stdio.h>
#include <string.h>

#include "phcalc.h"

#define BUF_SZ	512

int main_routine();		// Main routine loop
void test_1();			// Run tests
void print_help();		// Print help

int main(int argc, char *argv[]){
	test_1();
	main_routine();
	_CrtDumpMemoryLeaks();
}

int main_routine() {
	phcalc_inst calc = phcalc_create_inst();
	char buf[BUF_SZ];
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
				//phcalc_strexpr(calc,e,buf,BUF_SZ);
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
					phcalc_strobj(&r,buf,BUF_SZ);
					phcalc_release_obj(calc,&r);
					printf("%s\n",buf);
				} else {
					printf("Error\n");
				}
				phcalc_expr_release(e);
			}
		} else if( strcmp(buf,"print")==0 ){
			phcalc_obj *obj = phcalc_getdef(calc,arg);
			if(obj!=0){
				phcalc_strobj(obj,buf,BUF_SZ);
				printf("%s\n",buf);
			}
		} else if( strcmp(buf,"csv-load")==0 ){
			char *fields[32];
			char *s2 = strchr(arg,' ');
			if(s2!=0){
				FILE *file;
				int i = 0;
				arg[s2-arg] = 0;
				s2 ++;
				fields[i++] = s2;
				while( s2 = strchr(s2,',') ){
					s2[0] = 0;
					s2 ++;
					fields[i++] = s2;
				}
				file = fopen(arg,"rt");
				if(file != 0){
					phcalc_csv_load(calc,file,fields,i,0);
					fclose(file);
				} else
					printf("File is not found");
			} else
				printf("Incorrect command");
		// } else if( strcmp(buf,"csv-save")==0 ){
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
		"print - print definition\n"
		"        print x\n"
		"csv-load - load csv file\n"
		"        csv-load filename.csv x,y,z\n"
		"help  - help\n"
		"exit  - exit\n"
	);
}